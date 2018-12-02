//
//  TTPacket.cpp
//  TTPlayerExample
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "TTPacket.hpp"

using namespace TT;

Packet::Packet()
: _avpacket(nullptr)
, _data(nullptr)
, _capacity(0)
, _size(0)
{
    
}

Packet::Packet(AVPacket *avpacket)
: _avpacket(avpacket)
, _data(nullptr)
, _capacity(0)
, _size(0)
{
    if (_avpacket) {
        pts = _avpacket->pts;
        dts = _avpacket->dts;
        pos = _avpacket->pos;
    }
}

Packet::~Packet() {
    if (_avpacket) {
        av_packet_unref(_avpacket);
        av_free(_avpacket);
        _avpacket = NULL;
    }
    
    if (_data) {
        free(_data);
        _data = nullptr;
    }
}

bool Packet::allocData(size_t capacity) {
    if (_data) {
        free(_data);
        _data = nullptr;
    }
    
    _capacity = capacity;
    _data = (uint8_t *)malloc(capacity);
    
    return true;
}

void Packet::setsize(size_t size) {
    if (_avpacket) {
        _size = _avpacket->size;
    } else {
        _size = size;
    }
}

uint8_t *Packet::data() {
    if (_avpacket) {
        return _avpacket->data;
    } else {
        return _data;
    }
}

size_t Packet::size() {
    if (_avpacket) {
        return _avpacket->size;
    } else {
        return _size;
    }
}



