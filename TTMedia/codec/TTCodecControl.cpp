//
//  TTCodecControl.cpp
//  TTPlayerExample
//
//  Created by liang on 2/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTCodecControl.hpp"
#include "TTMessageLoop.hpp"
#include "TTPacket.hpp"
#include "TTFrame.hpp"
#include "TTStream.hpp"
#include "TTCodec.hpp"
#include "TTVideoCodec.hpp"
#include "TTAudioCodec.hpp"

using namespace TT;

const static int kMaxFrameCount = 3;

CodecControl::CodecControl()
: _codec(nullptr)
, _stream(nullptr)
, _frameQueue(nullptr)
, _packetQueue(nullptr)
{
    _loop = std::make_shared<MessageLoop>();
    _loop->setMessageHandle(std::bind(&CodecControl::handleMessage, this, std::placeholders::_1));
    initMessages();
    _loop->start();
}

CodecControl::~CodecControl() {
    _loop->stop();
}

bool CodecControl::start(std::shared_ptr<Stream> stream) {
    if (stream == nullptr) {
        return false;
    }
    
    loop()->postMessage(std::make_shared<Message>(kCodecOpen, [&](std::shared_ptr<Message>) {
        // TODO check return value wether fail, notify observer.
        if (open(stream)) {
            std::shared_ptr<CodecObserver> ob = _observer.lock();
            if (ob) {
                ob->opened();
            }
        }
    }));
    return true;
}

bool CodecControl::stop() {
    _loop->emitMessage(kCodecClose);
    return true;
}

void CodecControl::initMessages() {
    _loop->signalMessage(std::make_shared<Message>(kCodecDecode, [&](std::shared_ptr<Message>) {
        decodePacket();
    }));
    
    _loop->signalMessage(std::make_shared<Message>(kCodecClose, [&](std::shared_ptr<Message>) {
        _codec->close();
        _codec = nullptr;
        _frameQueue->close();
        
        std::shared_ptr<CodecObserver> ob = _observer.lock();
        if (ob) {
            ob->closed();
        }
    }));
}

void CodecControl::handleMessage(std::shared_ptr<Message> message) {
    
}

bool CodecControl::open(std::shared_ptr<Stream> stream) {
    _stream = stream;
    _frameQueue->clear();
    if (_codec && _codec->open()) {
        return true;
    }
    
    return false;
}


#pragma mark - VideoCodecControl
VideoCodecControl::VideoCodecControl() {
    setframeQueue(std::make_shared<FrameQueue>("video_frame_queue", kMaxFrameCount));
}

VideoCodecControl::~VideoCodecControl() {

}

bool VideoCodecControl::open(std::shared_ptr<Stream> stream) {
    std::shared_ptr<VideoCodec> codec = std::make_shared<VideoCodec>(stream->internalStream());
    setcodec(codec);
    return CodecControl::open(stream);
}

void VideoCodecControl::decodePacket() {
    if (codec() == nullptr) {
        return;
    }
    
    std::shared_ptr<Packet> packet = packetQueue()->pop();
    if (packet && codec()) {
        std::shared_ptr<Frame> frame;
        frame = codec()->decode(packet);
        if (frame) {
            // Reorder with pts for B frame.
            LOG(TRACE) << "Reorder begin " << frame->pts;
            frameQueue()->insert(frame, [&](std::shared_ptr<Frame> l, std::shared_ptr<Frame> r) -> bool {
                LOG(TRACE) << "Reorder " << l->pts << " " << r->pts;
                return l->pts <= r->pts;
            });
            LOG(TRACE) << "Reorder end " << frame->pts;
        }
    }
    
    loop()->emitMessage(kCodecDecode);
}

#pragma mark - AudioCodecControl
AudioCodecControl::AudioCodecControl() {
    setframeQueue(std::make_shared<FrameQueue>("audio_frame_queue", kMaxFrameCount));
}

AudioCodecControl::~AudioCodecControl() {
    
}

bool AudioCodecControl::open(std::shared_ptr<Stream> stream) {
    std::shared_ptr<AudioCodec> codec = std::make_shared<AudioCodec>(stream->internalStream());
    setcodec(codec);
    return CodecControl::start(stream);
}

void AudioCodecControl::decodePacket() {
    if (codec() == nullptr) {
        return;
    }
    
    std::shared_ptr<Packet> packet = packetQueue()->pop();
    if (packet && codec()) {
        std::shared_ptr<Frame> frame;
        frame = codec()->decode(packet);
        if (frame) {
            frameQueue()->push(frame);
        }
    }
    
    loop()->emitMessage(kCodecDecode);
}
