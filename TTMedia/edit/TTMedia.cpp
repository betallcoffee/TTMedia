//
//  TTMedia.cpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"
#include "TTdef.h"
#include "TTMedia.hpp"
#include "TTMutex.hpp"

#include "TTHTTPIO.hpp"
#include "TTFFDemuxer.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

#include "TTFFWriter.hpp"

using namespace TT;

const static int kMaxFrameCount = 0;

Media::Media(std::shared_ptr<URL> url)
: Material(MaterialType::kVideo, url)
, _mutex(PTHREAD_MUTEX_INITIALIZER)
, _stream(nullptr), _demuxer(nullptr), _writer(nullptr)
, _videoCodec(nullptr), _audioCodec(nullptr)
, _vFrameArray("video_frame_array", kMaxFrameCount)
, _aFrameArray("audio_frame_array", kMaxFrameCount)
, _previews("video_preview_frame", kMaxFrameCount) {
    av_register_all();
    avformat_network_init();
    el::Loggers::setLoggingLevel(el::Level::Debug);
    LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
}

Media::~Media() {
}

bool Media::open() {
    Mutex m(&_mutex);
    if (isOpen()) {
        return true;
    }
    
    _demuxer = TT_MK_SP(FFDemuxer)();
    _demuxer->open(url());
    
    if (_demuxer->hasAudio()) {
        _audioCodec = TT_MK_SP(AudioCodec)(_demuxer->audioStream()->internalStream());
        _audioCodec->open();
    }
    
    if (_demuxer->hasVideo()) {
        _videoCodec = TT_MK_SP(VideoCodec)(_demuxer->videoStream()->internalStream());
        _videoCodec->open();
    }
    
    setisOpen(true);
    setisEnd(false);
    return true;
}

bool Media::close() {
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
    
    setisOpen(false);
    return true;
}

void Media::save(std::shared_ptr<URL> url) {
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

bool Media::loadMore() {
    Mutex m(&_mutex);
    if (!isOpen()) {
        return false;
    }
    
    size_t oldSize = _previews.size();
    while (readData()) {
        size_t newSize = _previews.size();
        if (newSize > oldSize) {
            setisEnd(false);
            return true;
        }
    }
    
    if (_demuxer->isEOF()) {
        setisEnd(true);
        return false;
    } else {
        return true;
    }
}

int Media::audioFrameCount() {
    Mutex m(&_mutex);
    return static_cast<int>(_aFrameArray.size());
}

std::shared_ptr<Frame> Media::audioFrame(int index) {
    Mutex m(&_mutex);
    if (0 <= index && index < _aFrameArray.size()) {
        return _aFrameArray[index];
    }
    return nullptr;
}

int Media::frameCount() {
    Mutex m(&_mutex);
    return static_cast<int>(_vFrameArray.size());
}

std::shared_ptr<Frame> Media::frame(int index) {
    Mutex m(&_mutex);
    if (0 <= index && index < _vFrameArray.size()) {
        return _vFrameArray[index];
    }
    return nullptr;
}

int Media::previewCount() {
    Mutex m(&_mutex);
    return static_cast<int>(_previews.size());
}

std::shared_ptr<Frame> Media::preview(int index) {
    Mutex m(&_mutex);
    if (0 <= index && index < _previews.size()) {
        return _previews[index];
    }
    return nullptr;
}

bool Media::readData() {
    std::shared_ptr<Packet> packet = _demuxer->read();
    if (packet) {
        switch (packet->type) {
            case kPacketTypeAudio:
                audioDecode(packet);
                break;
            case kPacketTypeVideo:
                videoDecode(packet);
                break;
            default:
                break;
        }
    } else if (_demuxer->isEOF()) {
        return false;
    }
    return true;
}

bool Media::audioDecode(std::shared_ptr<Packet> packet) {
    if (packet && _audioCodec) {
        std::shared_ptr<Frame> frame;
        frame = _audioCodec->decode(packet);
        if (frame) {
            LOG(TRACE) << "Decode audio frame pts:" << frame->pts;
            _aFrameArray.pushBack(frame);
            return true;
        }
    }
    return false;
}

bool Media::videoDecode(std::shared_ptr<Packet> packet) {
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

bool Media::writeData() {
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
