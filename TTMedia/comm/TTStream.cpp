//
//  TTStream.cpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTStream.hpp"

using namespace TT;

TT_PROPERTY_IMPL(Stream, int, index)
TT_PROPERTY_IMPL(Stream, StreamType, type)
TT_PROPERTY_IMPL(Stream, CodecID, codecID)
TT_PROPERTY_IMPL(Stream, uint32_t, timeScale)
TT_PROPERTY_IMPL(Stream, uint64_t, duration)
TT_PROPERTY_IMPL(Stream, uint8_t *, extraData);
TT_PROPERTY_IMPL(Stream, size_t, extraSize);

Stream::Stream()
: _extraData(nullptr)
, _extraSize(0)
{
    
}

Stream::Stream(AVStream *stream)
: _extraData(nullptr)
, _extraSize(0)
{
    _internalStream = stream;
}

Stream::~Stream() {
    _internalStream = nullptr;
    
    if (_extraData) {
        free(_extraData);
        _extraData = nullptr;
    }
}

bool Stream::allocExtraData(size_t size) {
    if (size <= 0) {
        return false;
    }
    
    if (_extraData) {
        free(_extraData);
        _extraData = nullptr;
    }
    
    _extraData = (uint8_t *)malloc(size);
    return true;
}
