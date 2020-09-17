//
//  TTStream.cpp
//  TTMedia
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTStream.hpp"

using namespace TT;

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
