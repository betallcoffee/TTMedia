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

const static int kMaxPacketCount = 300;
const static int kMaxFrameCount = 0;

Video::Video() :
Material(MaterialType::kVideo),
_previews("video_preview_frame", kMaxFrameCount),
_statusM(static_cast<int>(VideoStatus::kNone)),
_statusCallback(nullptr), _eventCallback(nullptr), _readFrameCallback(nullptr),
_stream(nullptr), _demuxer(nullptr), _writer(nullptr),
_vPacketQueue("video_packet_queue", kMaxPacketCount), _aPacketQueue("audio_packet_queue", kMaxPacketCount),
_vFrameArray("video_frame_array", kMaxFrameCount), _aFrameQueue("audio_frame_queue", kMaxFrameCount),
_vPacketArray("video_packet_array", kMaxFrameCount),
_demuxMutex(PTHREAD_MUTEX_INITIALIZER),
_audioMutex(PTHREAD_MUTEX_INITIALIZER),
_videoMutex(PTHREAD_MUTEX_INITIALIZER) {
    av_register_all();
    avformat_network_init();
    el::Loggers::setLoggingLevel(el::Level::Debug);
    LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
    registerStatus();
}

Video::~Video() {
}

void Video::registerStatus() {
    _statusM.registerStatusHandle(static_cast<int>(VideoStatus::kOpen),
                                  std::bind(&Video::openStatus, this, std::placeholders::_1));
    _statusM.registerStatusHandle(static_cast<int>(VideoStatus::kClose),
                                  std::bind(&Video::closeStatus, this, std::placeholders::_1));
    _statusM.registerStatusHandle(static_cast<int>(VideoStatus::kRead),
                                  std::bind(&Video::readStatus, this, std::placeholders::_1));
    _statusM.registerStatusHandle(static_cast<int>(VideoStatus::kWrite),
                                  std::bind(&Video::writeStatus, this, std::placeholders::_1));
    
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kNone), static_cast<int>(VideoStatus::kOpen));
    
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kOpen), static_cast<int>(VideoStatus::kClose));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kOpen), static_cast<int>(VideoStatus::kError));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kOpen), static_cast<int>(VideoStatus::kRead));
    
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kClose), static_cast<int>(VideoStatus::kStoped));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kClose), static_cast<int>(VideoStatus::kError));
    
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kRead), static_cast<int>(VideoStatus::kClose));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kRead), static_cast<int>(VideoStatus::kError));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kRead), static_cast<int>(VideoStatus::kEdit));
    
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kEdit), static_cast<int>(VideoStatus::kClose));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kEdit), static_cast<int>(VideoStatus::kError));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kEdit), static_cast<int>(VideoStatus::kWrite));
    
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kWrite), static_cast<int>(VideoStatus::kClose));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kWrite), static_cast<int>(VideoStatus::kError));
    _statusM.addStatusTransfer(static_cast<int>(VideoStatus::kWrite), static_cast<int>(VideoStatus::kEdit));
}

bool Video::process() {
    return _statusM.handleForStatus();
}

bool Video::open(std::shared_ptr<URL> url) {
    _url = url;
    setStatus(VideoStatus::kOpen);
    return true;
}

bool Video::close() {
    setStatus(VideoStatus::kClose);
    _aPacketQueue.clear();
    _vPacketQueue.clear();
    return true;
}

void Video::save(std::shared_ptr<URL> url) {
    _saveUrl = url;
    setStatus(VideoStatus::kWrite);
}

int Video::frameCount() {
    return static_cast<int>(_vFrameArray.size());
}

std::shared_ptr<Frame> Video::frame(int index) {
    if (0 <= index && index < frameCount()) {
        return _vFrameArray[index];
    }
    return nullptr;
}

void Video::setStatusCallback(StatusCallback cb) {
//    Mutex m(&_statusMutex);
    _statusCallback = cb;
}

void Video::setEventCallback(EventCallback cb) {
//    Mutex m(&_statusMutex);
    _eventCallback = cb;
}

void Video::setReadFrameCallback(ReadFrameCallback cb) {
    Mutex m(&_videoMutex);
    _readFrameCallback = cb;
}

int Video::previewCount() {
    return (int)_previews.size();
}

std::shared_ptr<Frame> Video::preview(int index) {
    if (0 <= index && index < previewCount()) {
        return _previews[index];
    }
    return nullptr;
}

void Video::setStatus(VideoStatus status) {
    _statusM.transferToStatus(static_cast<int>(status));
//    if (oldStatus != _status && _statusCallback) {
//        LOG(DEBUG) << "Did change status " << (int)oldStatus << " to " << (int)_status;
//        _statusCallback(this, _status);
//    }
}

bool Video::openStatus(int status) {
    if (status == static_cast<int>(VideoStatus::kOpen)) {
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
        
        setStatus(VideoStatus::kRead);
        return true;
    }
    
    return false;
}

bool Video::closeStatus(int status) {
    if (status == static_cast<int>(VideoStatus::kClose)) {
        _aPacketQueue.clear();
        _vPacketQueue.clear();
        _aFrameQueue.clear();
        _vFrameArray.clear();
        
        _aPacketQueue.close();
        if (_audioCodec) {
            _audioCodec->close();
            _audioCodec.reset();
        }
        
        _vPacketQueue.close();
        if (_videoCodec) {
            _videoCodec->close();
            _videoCodec.reset();
        }
        
        if (_demuxer) {
            _demuxer->close();
            _demuxer.reset();
        }
        
        if (_writer) {
            _writer->close();
            _writer.reset();
        }
        
        setStatus(VideoStatus::kStoped);
        return true;
    }
    
    return false;
}

bool Video::readStatus(int status) {
    std::shared_ptr<Packet> packet = _demuxer->read();
    if (packet) {
        switch (packet->type) {
            case kPacketTypeAudio:
//                _aPacketQueue.push(packet);
                break;
            case kPacketTypeVideo:
                videoDecode(packet);
//                _vPacketArray.pushBack(packet);
                break;
            default:
                break;
        }
        return true;
    } else if (_demuxer->isEOF()) {
        if (_eventCallback) {
            _eventCallback(this, VideoEvent::kReadEnd);
        }
        setStatus(VideoStatus::kEdit);
        return true;
    }
    
    return false;
}

void Video::videoDecode(std::shared_ptr<Packet> packet) {
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
            
            if (_readFrameCallback) {
                _readFrameCallback(this, _vFrameArray.size());
            }
        }
    }
}

bool Video::writeStatus(int status) {
    if (_demuxer && _writer == nullptr) {
        _writer = std::make_shared<FFWriter>();
        if (!_writer->open(_saveUrl, _demuxer->audioCodecParams(), _demuxer->videoCodecParams())) {
            LOG(ERROR) << "Edit muxer open failed:" << _saveUrl;
            _writer->cancel();
            _writer = nullptr;
            setStatus(VideoStatus::kEdit);
            return true;
        }
    }
    
    if (_writer) {
        int count = (int)_vFrameArray.size();
        for (int i = 10; i < count; i++) {
            std::shared_ptr<Frame> frame = _vFrameArray[i];
            _writer->processFrame(frame);
        }
        setStatus(VideoStatus::kClose);
        return true;
    }
    
    return false;
}

bool Video::encode() {
    return false;
}
