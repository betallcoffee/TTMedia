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
}

CodecControl::~CodecControl() {
    
}

bool CodecControl::start(std::shared_ptr<Stream> stream) {
    if (stream == nullptr) {
        return false;
    }
    
    _stream = stream;
    return true;
}

bool CodecControl::stop() {
    return false;
}

void CodecControl::initMessages() {
    _loop->signalMessage(std::make_shared<Message>(kCodecDecode, [&](std::shared_ptr<Message>) {
        this->decodePacket();
    }));
}

void CodecControl::handleMessage(std::shared_ptr<Message> message) {
    
}


#pragma mark - VideoCodecControl
VideoCodecControl::VideoCodecControl() {
    
}

VideoCodecControl::~VideoCodecControl() {

}

bool VideoCodecControl::start(std::shared_ptr<Stream> stream) {
    if (!CodecControl::start(stream)) {
        return false;
    }
    
    setframeQueue(std::make_shared<FrameQueue>("video_frame_queue", kMaxFrameCount));
    std::shared_ptr<VideoCodec> codec = std::make_shared<VideoCodec>(stream->internalStream());
    setcodec(codec);
    return codec->open();
}

void VideoCodecControl::decodePacket() {
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
    
}

AudioCodecControl::~AudioCodecControl() {
    
}

bool AudioCodecControl::start(std::shared_ptr<Stream> stream) {
    if (!CodecControl::start(stream)) {
        return false;
    }
    
    setframeQueue(std::make_shared<FrameQueue>("audio_frame_queue", kMaxFrameCount));
    std::shared_ptr<AudioCodec> codec = std::make_shared<AudioCodec>(stream->internalStream());
    setcodec(codec);
    return codec->open();
}

void AudioCodecControl::decodePacket() {
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
