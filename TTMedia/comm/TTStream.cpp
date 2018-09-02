//
//  TTStream.cpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTStream.hpp"

using namespace TT;

Stream::Stream() {
    
}

Stream::Stream(AVStream *stream) {
    _internalStream = stream;
}

Stream::~Stream() {
    _internalStream = nullptr;
}
