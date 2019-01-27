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
    _videoPts = 0;
    int ret = avformat_alloc_output_context2(&_formatContext, nullptr, nullptr, _url->cStr());
    if (ret < 0) {
        LOG(ERROR) << "Muxer FFMuxer open failed:" << av_err2str(ret) << " url:" << url;
        close();
        return false;
    }
    
//    if (audioCodecParam) {
//        AVCodec *codec = avcodec_find_encoder(audioCodecParam->codec_id);
//        _audioStream = avformat_new_stream(_formatContext, codec);
//        if (_audioStream) {
//            avcodec_parameters_copy(_audioStream->codecpar, audioCodecParam);
//        }
//    }
    
    AVCodecParameters *avVideoCodecParam = nullptr;
    if (videoCodecParams) {
        avVideoCodecParam = avcodec_parameters_alloc();
        avVideoCodecParam->codec_type = AVMEDIA_TYPE_VIDEO;
        avVideoCodecParam->width = videoCodecParams->width;
        avVideoCodecParam->height = videoCodecParams->height;
        if (videoCodecParams->codecID == CodecID::kCodecIDH264) {
            avVideoCodecParam->codec_id = AV_CODEC_ID_H264;
        }
        avVideoCodecParam->profile = videoCodecParams->profile;
        avVideoCodecParam->bit_rate = videoCodecParams->bitRate;
        
        AVCodec *codec = avcodec_find_encoder(avVideoCodecParam->codec_id);
        // https://stackoverflow.com/questions/33981707/ffmpeg-copy-streams-without-transcode
        unsigned int tag = 0;
        if (av_codec_get_tag2(_formatContext->oformat->codec_tag, avVideoCodecParam->codec_id, &tag) == 0) {
            LOG(ERROR) << "Muxer FFMuxer av_codec_get_tag2 failed: " << avVideoCodecParam->codec_id;
        }
        avVideoCodecParam->codec_tag = tag;
        _videoStream = avformat_new_stream(_formatContext, codec);
        if (_videoStream) {
            avcodec_parameters_copy(_videoStream->codecpar, avVideoCodecParam);
            _videoStream->start_time = 0;
            _videoStream->time_base.num = videoCodecParams->timeBase.num;
            _videoStream->time_base.den = videoCodecParams->timeBase.den;
            _videoCodecParams = videoCodecParams;
            _videoCodec = std::make_shared<VideoCodec>(videoCodecParams, _videoStream, kVideoCodecEncode);
            _videoCodec->setEncodeFrameCallback(std::bind(&FFMuxer::encodeFrameCallback, this, std::placeholders::_1));
            _videoCodec->open();
        }
    }
    
    av_dump_format(_formatContext, 0, _url->cStr(), 1);
    
    if (!(_formatContext->flags & AVFMT_NOFILE)) {
        ret = avio_open(&_formatContext->pb, _url->cStr(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOG(ERROR) << "Could not open output file:" << _url->cStr() << " error:" << av_err2str(ret);
            close();
            if (avVideoCodecParam) {
                avcodec_parameters_free(&avVideoCodecParam);
            }
            return false;
        }
    }
    
    ret = avformat_write_header(_formatContext, NULL);
    if (ret < 0) {
        LOG(ERROR) << "Error when write header file:" << _url->cStr() << " error:" << av_err2str(ret);
        close();
        if (avVideoCodecParam) {
            avcodec_parameters_free(&avVideoCodecParam);
        }
        return false;
    }
    
    if (avVideoCodecParam) {
        avcodec_parameters_free(&avVideoCodecParam);
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

bool FFMuxer::write(std::shared_ptr<Frame> frame) {
    if (kMediaTypeVideo == frame->mediaType) {
        if (_videoCodec) {
            _videoCodec->encode(frame);
            return true;
        }
    }
    return false;
}

void FFMuxer::encodeFrameCallback(std::shared_ptr<Packet> packet) {
    packet->pts = _videoPts;
    int64_t pts = _videoPts * tt_q2d(_videoCodecParams->timeBase) / av_q2d(_videoStream->time_base);
    packet->avpacket()->pts = pts;
    packet->avpacket()->dts = pts;
    packet->avpacket()->stream_index = _videoStream->index;
    write(packet);
    _videoPts++;
}
