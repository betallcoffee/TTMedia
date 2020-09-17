//
//  TTAudioCodec.cpp
//  TTMedia
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include <algorithm>

#include "easylogging++.h"

#include "TTAudioCodec.hpp"
#include "TTFrame.hpp"
#include "TTPacket.hpp"

using namespace TT;

AudioCodec::AudioCodec(const AVStream *avStream, CodecType type)
: _type(type)
, _avStream(avStream)
, _avCodec(nullptr)
, _avFrame(nullptr)
, _swrContext(nullptr)
, _sampleRate(0)
, _channelLayout(0)
, _fmt(AV_SAMPLE_FMT_NONE)
, _nbSamples(0)
, _cb(nullptr)
{
}

AudioCodec::AudioCodec(std::shared_ptr<CodecParams> codecParams,
                       const AVStream *avStream,
                       CodecType type)
: _type(type)
, _codecParams(codecParams)
, _avStream(avStream)
, _avCodec(nullptr)
, _avFrame(nullptr)
, _swrContext(nullptr)
, _sampleRate(0)
, _channelLayout(0)
, _fmt(AV_SAMPLE_FMT_NONE)
, _nbSamples(0)
, _cb(nullptr)
{
}

AudioCodec::~AudioCodec() {
    close();
}

bool AudioCodec::open() {
    if (_avStream) {
        if (CodecType::kDecode == _type) {
            _avCodecContext = _avStream->codec;
            if (_avCodecContext) {
                _avCodec = avcodec_find_decoder(_avCodecContext->codec_id);
                if (_avCodec) {
                    if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                        _avFrame = av_frame_alloc();
                        return true;
                    }
                }
            }
        } else {
            _avCodec = avcodec_find_encoder(_avStream->codecpar->codec_id);
            if (_avCodec && _codecParams) {
                _avCodecContext = avcodec_alloc_context3(_avCodec);
                _avCodecContext->codec_type = _avCodec->type;
                _avCodecContext->codec_id = _avStream->codecpar->codec_id;
                
                _avCodecContext->channels = _codecParams->channels;
                _avCodecContext->channel_layout = _avStream->codecpar->channel_layout;
                _avCodecContext->sample_rate = _codecParams->sampleRate;
                
                _avCodecContext->time_base.num = _codecParams->timeBase.num;
                _avCodecContext->time_base.den = _codecParams->timeBase.den;
                _avCodecContext->bit_rate = _codecParams->bitRate;
                _avCodecContext->sample_fmt = AV_SAMPLE_FMT_FLT;
                
                int ret = avcodec_open2(_avCodecContext, _avCodec, NULL);
                if (ret != 0) {
                    // TODO destroy
                    av_log(NULL, AV_LOG_ERROR, "audio avcodec_open2 error: %s", av_err2str(ret));
                    return false;
                } else {
                    avcodec_parameters_from_context(_avStream->codecpar, _avCodecContext);
                    return true;
                }
            }
        }
    }
    
    return false;
}

void AudioCodec::close() {
    if (_avFrame) {
        av_frame_free(&_avFrame);
        _avFrame = nullptr;
    }
    if (_avCodecContext) {
        avcodec_close(_avCodecContext);
        _avCodec = nullptr;
    }
}

void AudioCodec::flush() {
    std::shared_ptr<Frame> frame = std::make_shared<Frame>();
    encode(frame);
}

void AudioCodec::createSwrContext() {
    if (_swrContext) {
        swr_close(_swrContext);
        swr_free(&_swrContext);
        _swrContext = NULL;
    }
    
    _swrContext = swr_alloc();
    swr_alloc_set_opts(_swrContext,
                       _channelLayout,
                       AV_SAMPLE_FMT_FLT,
                       _sampleRate,
                       _channelLayout,
                       _fmt,
                       _sampleRate,0, NULL);
    swr_init(_swrContext);
}

std::shared_ptr<Frame> AudioCodec::decode(std::shared_ptr<Packet> packet) {
    int gotFrame = 0;
    int ret = avcodec_decode_audio4(_avCodecContext, _avFrame, &gotFrame, packet->avpacket());
    if (ret < 0)
    {
        printf("Error decoding audio frame (%d), (%s)", ret, av_err2str(ret));
    }
    
    if (gotFrame) {
        int64_t dec_channel_layout =
        (_avFrame->channel_layout && av_frame_get_channels(_avFrame) == av_get_channel_layout_nb_channels(_avFrame->channel_layout)) ?
        _avFrame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(_avFrame));
        
        if (_swrContext             == NULL            ||
            _avFrame->format        != _fmt            ||
            dec_channel_layout      != _channelLayout ||
            _avFrame->sample_rate   != _sampleRate) {
            _fmt = (AVSampleFormat)_avFrame->format;
            _channelLayout = dec_channel_layout;
            _sampleRate = _avFrame->sample_rate;
            createSwrContext();
            if (_cb) {
                AudioDesc desc;
                desc.sampleRate = _sampleRate;
                desc.channels = _avFrame->channels;
                desc.fmt = _fmt;
                
                _nbSamples = std::max(_avCodecContext->frame_size, _avFrame->nb_samples);
                desc.nbSamples = _nbSamples;
                _cb(desc);
            }
        }
        
        AVFrame *resampleFrame = av_frame_alloc();
        resampleFrame->channels = _avFrame->channels;
        resampleFrame->channel_layout = _avFrame->channel_layout;
        resampleFrame->sample_rate = _avFrame->sample_rate;
        resampleFrame->nb_samples = _avFrame->nb_samples;
        resampleFrame->format = AV_SAMPLE_FMT_FLT;
        av_frame_get_buffer(resampleFrame, 0);
        
        int nbSamples = swr_convert(_swrContext,
                                    resampleFrame->data,
                                    resampleFrame->linesize[0],
                                    (const uint8_t **)_avFrame->data,
                                    _avFrame->nb_samples);
        if (nbSamples < 0) {
            av_log(NULL, AV_LOG_ERROR, "swr_convert audio frame error: %s\n", av_err2str(nbSamples));
            av_frame_free(&resampleFrame);
            return nullptr;
        } else {
            std::shared_ptr<Frame> frame = std::make_shared<Frame>(resampleFrame, kMediaTypeAudio, kAudioPCM);
            if (frame == nullptr) {
                av_frame_free(&resampleFrame);
            }
            frame->pts = packet->pts;
            frame->sampleFormat = _fmt;
            if (_avFrame->pts != AV_NOPTS_VALUE) {
                AVRational tb = av_codec_get_pkt_timebase(_avCodecContext);
                frame->pts = _avFrame->pts * av_q2d(tb) * 1000;
            } else {
                AVRational tb = _avStream->time_base;
                frame->pts = _avFrame->pkt_pts * av_q2d(tb) * 1000;
            }
            
            return frame;
        }
    }
    
    return nullptr;
}

bool AudioCodec::encode(std::shared_ptr<Frame> frame) {
    if (_avCodecContext == nullptr) {
        LOG(ERROR) << "Codec AudioCodec encode avcodeccontext is null";
        return false;
    }
    
    int rsend = avcodec_send_frame(_avCodecContext, frame->avFrame());
    if (rsend < 0 && AVERROR(EAGAIN) != rsend) {
        LOG(ERROR) << "Codec AudioCodec encode send frame error: " << av_err2str(rsend);
        return false;
    }
    
    do {
        AVPacket *avpacket = av_packet_alloc();
        int rrecv = avcodec_receive_packet(_avCodecContext, avpacket);
        if (rrecv != 0) {
            LOG(ERROR) << "Codec AudioCodec encode receive packet error: " << av_err2str(rrecv);
            av_packet_free(&avpacket);
            if (AVERROR(EAGAIN) == rrecv) {
                LOG(WARNING) << "Codec AudioCodec encode need receive after send frame again";
                if (AVERROR(EAGAIN) == rsend) {
                    LOG(INFO) << "Codec AudioCodec encode try to send input";
                    rsend = avcodec_send_frame(_avCodecContext, frame->avFrame());
                    continue;
                } else {
                    break;
                }
            } else if (AVERROR_EOF == rrecv) {
                LOG(INFO) << "Codec AudioCodec encode eof";
                break;
            }
        } else {
            if (rrecv == 0 && _encodeFrameCallback) {
                std::shared_ptr<Packet> packet = std::make_shared<Packet>(avpacket);
                if (frame->avFrame()) {
                    packet->audioSamplePerPacket = frame->avFrame()->nb_samples;
                }
                _encodeFrameCallback(packet);
                continue;
            } else {
                if (!_encodeFrameCallback) {
                    LOG(WARNING) << "Codec AudioCodec encode did not has encodeFrameCallback";
                }
                av_packet_free(&avpacket);
                return false;
            }
        }
    } while (true);
    
    return true;
}


