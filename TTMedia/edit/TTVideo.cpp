//
//  TTVideoEdit.cpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTVideo.hpp"
#include "TTMutex.hpp"

#include "TTHTTPIO.hpp"
#include "TTFFDemuxer.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

#include "TTFFWriter.hpp"

using namespace TT;

const static int kMaxFrameCount = 0;

Video::Video() :
Material(MaterialType::kVideo),
_mutex(PTHREAD_MUTEX_INITIALIZER),
_stream(nullptr), _demuxer(nullptr), _writer(nullptr),
_videoCodec(nullptr), _audioCodec(nullptr),
_vFrameArray("video_frame_array", kMaxFrameCount),
_previews("video_preview_frame", kMaxFrameCount) {
    av_register_all();
    avformat_network_init();
    el::Loggers::setLoggingLevel(el::Level::Debug);
    LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
}

Video::~Video() {
}

bool Video::init() {
    return true;
}

bool Video::process() {
    return false;
}

bool Video::open(std::shared_ptr<URL> url) {
    Mutex m(&_mutex);
    _url = url;
    _demuxer = std::make_shared<FFDemuxer>();
    _demuxer->open(_url);
    
    if (_demuxer->hasAudio()) {
        _audioCodec = std::make_shared<AudioCodec>(_demuxer->audioStream()->internalStream());
        _audioCodec->open();
    }
    
    if (_demuxer->hasVideo()) {
        _videoCodec = std::make_shared<VideoCodec>(_demuxer->videoStream()->internalStream());
        _videoCodec->open();
    }
    
    return true;
}

bool Video::close() {
    Mutex m(&_mutex);
    _vFrameArray.clear();
    _previews.clear();
    
    if (_audioCodec) {
        _audioCodec->close();
        _audioCodec = nullptr;
    }
    
    if (_videoCodec) {
        _videoCodec->close();
        _videoCodec = nullptr;
    }
    
    if (_demuxer) {
        _demuxer->close();
        _demuxer = nullptr;
    }
    
    if (_writer) {
        _writer->close();
        _writer = nullptr;
    }
    return true;
}

void Video::save(std::shared_ptr<URL> url) {
    Mutex m(&_mutex);
    _saveUrl = url;
    if (_demuxer && _writer == nullptr) {
        _writer = std::make_shared<FFWriter>();
        if (!_writer->open(_saveUrl, _demuxer->audioCodecParams(), _demuxer->videoCodecParams())) {
            LOG(ERROR) << "Edit muxer open failed:" << _saveUrl;
            _writer->cancel();
            _writer = nullptr;
        }
    }
    
    if (_writer) {
        int count = (int)_vFrameArray.size();
        for (int i = 10; i < count; i++) {
            std::shared_ptr<Frame> frame = _vFrameArray[i];
            _writer->processFrame(frame);
        }
        close();
    }
}

bool Video::loadMore() {
    Mutex m(&_mutex);
    size_t oldSize = _previews.size();
    while (readData()) {
        size_t newSize = _previews.size();
        if (newSize > oldSize) {
            return true;
        }
    }
    return false;
}

int Video::frameCount() {
    Mutex m(&_mutex);
    return static_cast<int>(_vFrameArray.size());
}

std::shared_ptr<Frame> Video::frame(int index) {
    Mutex m(&_mutex);
    if (0 <= index && index < _vFrameArray.size()) {
        return _vFrameArray[index];
    }
    return nullptr;
}

int Video::previewCount() {
    Mutex m(&_mutex);
    return static_cast<int>(_previews.size());
}

std::shared_ptr<Frame> Video::preview(int index) {
    Mutex m(&_mutex);
    if (0 <= index && index < _previews.size()) {
        return _previews[index];
    }
    return nullptr;
}

void Video::setEventCallback(EventCallback cb) {
}

void Video::setReadFrameCallback(ReadFrameCallback cb) {
}

bool Video::openDemuxer() {
    _demuxer = std::make_shared<FFDemuxer>();
    _demuxer->open(_url);
    
    if (_demuxer->hasAudio()) {
        _audioCodec = std::make_shared<AudioCodec>(_demuxer->audioStream()->internalStream());
        _audioCodec->open();
    }
    
    if (_demuxer->hasVideo()) {
        _videoCodec = std::make_shared<VideoCodec>(_demuxer->videoStream()->internalStream());
        _videoCodec->open();
    }
    
    return true;
}

bool Video::closeMedia() {
    _vFrameArray.clear();
    _previews.clear();
    
    if (_audioCodec) {
        _audioCodec->close();
        _audioCodec = nullptr;
    }
    
    if (_videoCodec) {
        _videoCodec->close();
        _videoCodec = nullptr;
    }
    
    if (_demuxer) {
        _demuxer->close();
        _demuxer = nullptr;
    }
    
    if (_writer) {
        _writer->close();
        _writer = nullptr;
    }
    
    return true;
}

bool Video::readData() {
    std::shared_ptr<Packet> packet = _demuxer->read();
    if (packet) {
        switch (packet->type) {
            case kPacketTypeAudio:

                break;
            case kPacketTypeVideo:
                return videoDecode(packet);
            default:
                break;
        }
    } else if (_demuxer->isEOF()) {
        return false;
    }
    return true;
}

bool Video::videoDecode(std::shared_ptr<Packet> packet) {
    if (packet && _videoCodec) {
        std::shared_ptr<Frame> frame;
        frame = _videoCodec->decode(packet);
        if (frame) {
            _width = frame->width;
            _height = frame->height;
            // Reorder with pts for B frame.
            LOG(TRACE) << "Reorder begin " << frame->pts << " frame count:" << _vFrameArray.size();
            frame->tag = static_cast<int>(_vFrameArray.size());
            _vFrameArray.insert(frame, [&](std::shared_ptr<Frame> l, std::shared_ptr<Frame> r) -> bool {
                LOG(TRACE) << "Reorder " << l->pts << " " << r->pts;
                return l->pts <= r->pts;
            });
            
            LOG(TRACE) << "Reorder end " << frame->pts << " frame count:" << _vFrameArray.size();
            
            if (frame->isKeyframe()) {
                LOG(TRACE) << "Decode keyframe pts:" << frame->pts;
                _previews.pushBack(frame);
            }
            
            return true;
        }
    }
    
    return false;
}

bool Video::writeData() {
    if (_demuxer && _writer == nullptr) {
        _writer = std::make_shared<FFWriter>();
        if (!_writer->open(_saveUrl, _demuxer->audioCodecParams(), _demuxer->videoCodecParams())) {
            LOG(ERROR) << "Edit muxer open failed:" << _saveUrl;
            _writer->cancel();
            _writer = nullptr;
        }
    }
    
    if (_writer) {
        int count = (int)_vFrameArray.size();
        for (int i = 10; i < count; i++) {
            std::shared_ptr<Frame> frame = _vFrameArray[i];
            _writer->processFrame(frame);
        }
        close();
    }
    
    return true;
}

bool Video::encode() {
    return false;
}
