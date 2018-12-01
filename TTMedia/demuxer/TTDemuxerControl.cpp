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

TT_PROPERTY_IMPL(DemuxerControl, std::shared_ptr<Demuxer>, demuxer)
TT_PROPERTY_IMPL(DemuxerControl, Queue<std::shared_ptr<Packet>>, vPacketQueue)
TT_PROPERTY_IMPL(DemuxerControl, Queue<std::shared_ptr<Packet>>, aPacketQueue)
TT_PROPERTY_IMPL(DemuxerControl, std::weak_ptr<DemuxerObserver>, observer)

DemuxerControl::DemuxerControl(std::shared_ptr<URL> url)
: _isDemuxing(false), _demuxer(nullptr)
, _vPacketQueue("demuxer video queue", kMaxPacketCount)
, _aPacketQueue("demuxer audio queue", kMaxPacketCount)
{
    _url = url;
    _loop = std::make_shared<MessageLoop>("DemuxerControl");
    _loop->setMessageHandle(std::bind(&DemuxerControl::handleMessage, this, std::placeholders::_1));
    initMessages();
}

DemuxerControl::~DemuxerControl()
{
    
}

bool DemuxerControl::start() {
    _loop->emitMessage(kOpen);
    return true;
}

bool DemuxerControl::stop() {
    _loop->emitMessage(kClose);
    return true;
}

bool DemuxerControl::seek(int64_t millisecond) {
    _loop->postMessage(std::make_shared<Message>(kSeek, [&](std::shared_ptr<Message> message) {
        _demuxer->seek(millisecond);
    }));
    
    return true;
}

void DemuxerControl::initMessages() {
    _loop->signalMessage(std::make_shared<Message>(kOpen, [&](std::shared_ptr<Message> message) {
        _demuxer = Demuxer::createDemuxer(_url);
        _demuxer->open(_url);
        _isDemuxing = true;
        _loop->postMessage(kRead);
        std::shared_ptr<DemuxerObserver> observer = _observer.lock();
        if (observer) {
            observer->opened();
        }
    }));
    
    _loop->signalMessage(std::make_shared<Message>(kClose, [&](std::shared_ptr<Message> message) {
        _demuxer->close();
        _isDemuxing = false;
    }));
    
    _loop->signalMessage(std::make_shared<Message>(kRead, [&](std::shared_ptr<Message> message) {
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
                _aPacketQueue.push(packet);
                break;
            case kPacketTypeVideo:
                _vPacketQueue.push(packet);
                break;
            default:
                break;
        }
    } else {
        usleep(1000);
    }
    
    if (_isDemuxing) {
        _loop->postMessage(kRead);
    }
}


