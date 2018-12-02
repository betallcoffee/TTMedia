//
//  TTDemuxerThread.cpp
//  TTPlayerExample
//
//  Created by liang on 28/10/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTMessageLoop.hpp"

#include "TTDemuxerControl.hpp"
#include "TTDemuxer.hpp"

using namespace TT;

const static int kMaxPacketCount = 300;

DemuxerControl::DemuxerControl(std::shared_ptr<URL> url)
: _isDemuxing(false)
, _demuxer(nullptr)
{
    _url = url;
    _loop = std::make_shared<MessageLoop>("DemuxerControl");
    _loop->setMessageHandle(std::bind(&DemuxerControl::handleMessage, this, std::placeholders::_1));
    initMessages();
    _loop->start();
}

DemuxerControl::~DemuxerControl() {
    _loop->stop();
}

bool DemuxerControl::start() {
    _loop->emitMessage(kDemuxerOpen);
    return true;
}

bool DemuxerControl::stop() {
    _loop->emitMessage(kDemuxerClose);
    return true;
}

bool DemuxerControl::seek(int64_t millisecond) {
    _loop->postMessage(std::make_shared<Message>(kDemuxerSeek, [&](std::shared_ptr<Message> message) {
        _demuxer->seek(millisecond);
    }));
    
    return true;
}

void DemuxerControl::initMessages() {
    _loop->signalMessage(std::make_shared<Message>(kDemuxerOpen, [&](std::shared_ptr<Message> message) {
        _vPacketQueue = std::make_shared<PacketQueue>("video_packet_queue", kMaxPacketCount);
        _aPacketQueue = std::make_shared<PacketQueue>("audio_packet_queue", kMaxPacketCount);
        _demuxer = Demuxer::createDemuxer(_url);
        _demuxer->open(_url);
        _isDemuxing = true;
        _loop->postMessage(kDemuxerRead);
        std::shared_ptr<DemuxerObserver> ob = _observer.lock();
        if (ob) {
            ob->opened();
        }
    }));
    
    _loop->signalMessage(std::make_shared<Message>(kDemuxerClose, [&](std::shared_ptr<Message> message) {
        _demuxer->close();
        _isDemuxing = false;
    }));
    
    _loop->signalMessage(std::make_shared<Message>(kDemuxerRead, [&](std::shared_ptr<Message> message) {
        readPacket();
    }));
}

void DemuxerControl::handleMessage(std::shared_ptr<Message> message) {
    switch (message->code()) {
        default:
            break;
    }
}

void DemuxerControl::readPacket() {
    std::shared_ptr<Packet> packet = _demuxer->read();
    if (packet) {
        switch (packet->type) {
            case kPacketTypeAudio:
                _aPacketQueue->push(packet);
                break;
            case kPacketTypeVideo:
                _vPacketQueue->push(packet);
                break;
            default:
                break;
        }
    } else {
        usleep(1000);
    }
    
    if (_isDemuxing) {
        _loop->postMessage(kDemuxerRead);
    }
}


