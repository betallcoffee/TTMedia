//
//  TTFFDemuxer.cpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTMutex.hpp"
#include "TTFFDemuxer.hpp"

using namespace TT;

FFDemuxer::FFDemuxer()
: _url(nullptr)
, _mutex(PTHREAD_MUTEX_INITIALIZER)
, _isEOF(false)
, _formatContext(nullptr)
, _option(nullptr)
{
    
}

FFDemuxer::~FFDemuxer() {
    close();
}

int FFDemuxer::probe(std::shared_ptr<URL> url) {
    return 1;
}

bool FFDemuxer::open(std::shared_ptr<URL> url) {
    _url = url;
    _isEOF = false;
    _audioCodec = nullptr;
    _videoCodec = nullptr;
    _audioStream = nullptr;
    _videoStream = nullptr;
    
    _formatContext = avformat_alloc_context();
//    av_dict_set_int(&_option, "probesize", 100*1024, 0);
    
//    _formatContext->interrupt_callback.callback = read_interrupt_cb;
//    _formatContext->interrupt_callback.opaque = (__bridge void *)(self);
    
    int ret;
    if((ret = avformat_open_input(&_formatContext, _url->cStr(), NULL, NULL)) != 0) {
        printf("Couldn't open input stream %d %s.\n", ret, av_err2str(ret));
        return false;
    }
    
    av_format_inject_global_side_data(_formatContext);
    
    if((ret = avformat_find_stream_info(_formatContext,NULL))<0){
        printf("Couldn't find stream information %d.\n", ret);
        return false;
    }
    
    av_dump_format(_formatContext, 0, _url->cStr(), 0);
    
    for(int i = 0; i<_formatContext->nb_streams; i++){
        int type = _formatContext->streams[i]->codec->codec_type;
        if (type == AVMEDIA_TYPE_AUDIO) {
            AVStream *stream = _formatContext->streams[i];
            _audioStream = std::make_shared<Stream>(stream);
            _audioCodecParams = std::make_shared<CodecParams>();
            _audioCodecParams->sampleRate = stream->codecpar->sample_rate;
            _audioCodecParams->channels = stream->codecpar->channels;
            _audioCodecParams->channelLayout = stream->codecpar->channel_layout;
        } else if (type == AVMEDIA_TYPE_VIDEO) {
            AVStream *stream = _formatContext->streams[i];
            _videoStream = std::make_shared<Stream>(stream);
            _videoCodecParams = std::make_shared<CodecParams>();
            _videoCodecParams->codecType = kMediaTypeVideo;
            _videoCodecParams->width = stream->codecpar->width;
            _videoCodecParams->height = stream->codecpar->height;
            if (stream->codecpar->codec_id == AV_CODEC_ID_H264) {
                _videoCodecParams->codecID = CodecID::kCodecIDH264;
            } else if (stream->codecpar->codec_id == AV_CODEC_ID_PNG) {
                _videoCodecParams->codecID = CodecID::kCodecIDPNG;
            }
        }
    }
    
    if (_audioStream == nullptr &&
        _videoStream == nullptr) {
        return false;
    }
    
    return true;
}

void FFDemuxer::close() {
    if (_formatContext) {
        avformat_close_input(&_formatContext);
    }
    _audioCodec = nullptr;
    _videoCodec = nullptr;
}

TT_SP(Packet) FFDemuxer::read() {
    Mutex m(&_mutex);
    if (_formatContext == nullptr) {
        return nullptr;
    }
    
    AVPacket *avpacket=(AVPacket *)av_malloc(sizeof(AVPacket));
    int ret = av_read_frame(_formatContext, avpacket);
    if (ret == 0) {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>(avpacket);
        if (_audioStream && avpacket->stream_index == _audioStream->internalStream()->index) {
            packet->type = kPacketTypeAudio;
        } else if (_videoStream && avpacket->stream_index == _videoStream->internalStream()->index) {
            packet->type = kPacketTypeVideo;
        }
        return packet;
    } else if (ret == AVERROR_EOF) {
        _isEOF = true;
    }
    
    return nullptr;
}

bool FFDemuxer::seek(uint64_t pos) {
    Mutex m(&_mutex);
    if (_formatContext == nullptr) {
        return nullptr;
    }
    
    return false;
}

std::shared_ptr<AudioCodec> FFDemuxer::audioCodec() {
    if (nullptr == _audioCodec) {
        _audioCodec = std::make_shared<AudioCodec>(_audioStream->internalStream());
    }
    
    return _audioCodec;
}

std::shared_ptr<VideoCodec> FFDemuxer::videoCodec() {
    if (nullptr == _videoCodec) {
        _videoCodec = std::make_shared<VideoCodec>(_videoStream->internalStream());
    }
    
    return _videoCodec;
}


