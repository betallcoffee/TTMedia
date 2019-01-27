//
//  TTFFMuxer.cpp
//  TTPlayerExample
//
//  Created by liang on 19/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTFFMuxer.hpp"

using namespace TT;

FFMuxer::FFMuxer() {
    
}

FFMuxer::~FFMuxer() {
    close();
}

bool FFMuxer::open(std::shared_ptr<URL> url,
          std::shared_ptr<CodecParams> audioCodecParams,
          std::shared_ptr<CodecParams> videoCodecParams) {
    _url = url;
    _audioPts = 0;
    _videoPts = 0;
    int ret = avformat_alloc_output_context2(&_formatContext, nullptr, nullptr, _url->cStr());
    if (ret < 0) {
        LOG(ERROR) << "Muxer FFMuxer open failed:" << av_err2str(ret) << " url:" << url;
        close();
        return false;
    }
    
    if (!createAudioStream(audioCodecParams)) {
        close();
        LOG(ERROR) << "create audio stream failed";
        return false;
    }
    
    if (!createVideoStream(videoCodecParams)) {
        close();
        LOG(ERROR) << "create audio stream failed";
        return false;
    }
    
    av_dump_format(_formatContext, 0, _url->cStr(), 1);
    
    if (!(_formatContext->flags & AVFMT_NOFILE)) {
        ret = avio_open(&_formatContext->pb, _url->cStr(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOG(ERROR) << "Could not open output file:" << _url->cStr() << " error:" << av_err2str(ret);
            close();
            return false;
        }
    }
    
    ret = avformat_write_header(_formatContext, NULL);
    if (ret < 0) {
        LOG(ERROR) << "Error when write header file:" << _url->cStr() << " error:" << av_err2str(ret);
        close();
        return false;
    }
    
    return true;
}

void FFMuxer::close() {
    LOG(INFO) << "Muxer FFMuxer close";
    if (_formatContext) {
        av_write_trailer(_formatContext);
        avformat_close_input(&_formatContext);
        avformat_free_context(_formatContext);
    }
}

void FFMuxer::flush() {
    LOG(INFO) << "Muxer FFMuxer flush";
    if (_videoCodec) {
        _videoCodec->flush();
    }
}

bool FFMuxer::write(std::shared_ptr<Frame> frame) {
    if (kMediaTypeVideo == frame->mediaType) {
        if (_videoCodec) {
            _videoCodec->encode(frame);
            return true;
        }
    } else if (kMediaTypeAudio == frame->mediaType) {
        if (_audioCodec) {
            _audioCodec->encode(frame);
            return true;
        }
    }
    return false;
}

bool FFMuxer::write(std::shared_ptr<Packet> packet) {
    if (packet == nullptr) {
        LOG(ERROR) << "Error packet is null";
        return false;
    }
    int ret = av_interleaved_write_frame(_formatContext, packet->avpacket());
    if (ret < 0) {
        LOG(ERROR) << "Error muxing packet";
        return false;
    }
    return true;
}

bool FFMuxer::createAudioStream(std::shared_ptr<CodecParams> codecParams) {
    if (codecParams == nullptr) {
        return true;
    }
    
    bool ret = false;
    if (codecParams != nullptr) {
        AVCodecParameters *avCodecParam = avcodec_parameters_alloc();
        if (kCodecIDAAC == codecParams->codecID) {
            avCodecParam->codec_id = AV_CODEC_ID_AAC;
        }
        avCodecParam->channels = codecParams->channels;
        avCodecParam->channel_layout = av_get_default_channel_layout(codecParams->channels);
        avCodecParam->sample_rate = codecParams->sampleRate;
        avCodecParam->bit_rate = codecParams->bitRate;
        
        AVCodec *codec = avcodec_find_encoder(avCodecParam->codec_id);
        _audioStream = avformat_new_stream(_formatContext, codec);
        if (_audioStream) {
            avcodec_parameters_copy(_audioStream->codecpar, avCodecParam);
            _audioStream->start_time = 0;
            _audioStream->time_base.num = codecParams->timeBase.num;
            _audioStream->time_base.den = codecParams->timeBase.den;
            _audioCodecParams = codecParams;
            _audioCodec = std::make_shared<AudioCodec>(_audioCodecParams, _audioStream, CodecType::kEncode);
            _audioCodec->setencodeFrameCallback(std::bind(&FFMuxer::encodeAudioFrameCallback, this, std::placeholders::_1));
            ret = _audioCodec->open();
        }
        
        if (avCodecParam) {
            avcodec_parameters_free(&avCodecParam);
        }
    }
    
    return ret;
}

bool FFMuxer::createVideoStream(std::shared_ptr<CodecParams> codecParams) {
    if (codecParams == nullptr) {
        return true;
    }
    
    bool ret = false;
    
    if (codecParams != nullptr) {
        AVCodecParameters *avCodecParam = nullptr;
        avCodecParam = avcodec_parameters_alloc();
        avCodecParam->codec_type = AVMEDIA_TYPE_VIDEO;
        avCodecParam->width = codecParams->width;
        avCodecParam->height = codecParams->height;
        if (CodecID::kCodecIDH264 == codecParams->codecID) {
            avCodecParam->codec_id = AV_CODEC_ID_H264;
        }
        avCodecParam->profile = codecParams->profile;
        avCodecParam->bit_rate = codecParams->bitRate;
        
        AVCodec *codec = avcodec_find_encoder(avCodecParam->codec_id);
        // https://stackoverflow.com/questions/33981707/ffmpeg-copy-streams-without-transcode
        unsigned int tag = 0;
        if (av_codec_get_tag2(_formatContext->oformat->codec_tag, avCodecParam->codec_id, &tag) == 0) {
            LOG(ERROR) << "Muxer FFMuxer av_codec_get_tag2 failed: " << avCodecParam->codec_id;
        }
        avCodecParam->codec_tag = tag;
        _videoStream = avformat_new_stream(_formatContext, codec);
        if (_videoStream) {
            avcodec_parameters_copy(_videoStream->codecpar, avCodecParam);
            _videoStream->start_time = 0;
            _videoStream->time_base.num = codecParams->timeBase.num;
            _videoStream->time_base.den = codecParams->timeBase.den;
            _videoCodecParams = codecParams;
            _videoCodec = std::make_shared<VideoCodec>(_videoCodecParams,
                                                       _videoStream,
                                                       CodecType::kEncode);
            _videoCodec->setEncodeFrameCallback(std::bind(&FFMuxer::encodeVideoFrameCallback, this, std::placeholders::_1));
            ret = _videoCodec->open();
        }
        
        if (avCodecParam) {
            avcodec_parameters_free(&avCodecParam);
        }
    }
    
    return ret;
}

void FFMuxer::encodeAudioFrameCallback(std::shared_ptr<Packet> packet) {
    packet->pts = _audioPts;
    int64_t pts = _audioPts * tt_q2d(_audioCodecParams->timeBase) / av_q2d(_audioStream->time_base);
    packet->avpacket()->pts = pts;
    packet->avpacket()->dts = pts;
    packet->avpacket()->stream_index = _audioStream->index;
    write(packet);
    _audioPts++; // TODO 1024
}

void FFMuxer::encodeVideoFrameCallback(std::shared_ptr<Packet> packet) {
    packet->pts = _videoPts;
    int64_t pts = _videoPts * tt_q2d(_videoCodecParams->timeBase) / av_q2d(_videoStream->time_base);
    packet->avpacket()->pts = pts;
    packet->avpacket()->dts = pts;
    packet->avpacket()->stream_index = _videoStream->index;
    write(packet);
    _videoPts++;
}
