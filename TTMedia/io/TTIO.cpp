//
//  TTIO.cpp
//  TTMedia
//
//  Created by liang on 4/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTIO.hpp"
#include "TTHTTPIO.hpp"
#include "TTFileIO.hpp"

using namespace TT;

IO::IO()
: _mutex(PTHREAD_MUTEX_INITIALIZER)
, _isOpened(false)
, _url(nullptr)
, _cond(&_mutex)
, _buffer(10 * 1024 * 1024)
{
    
}

IO::~IO() {
    
}

bool IO::canOpen(std::shared_ptr<URL> url) {
    return false;
}

std::shared_ptr<IO> IO::createIO(std::shared_ptr<URL> url) {
    if (HTTPIO::canOpen(url)) {
        return std::make_shared<HTTPIO>();
    } else if (FileIO::canOpen(url)) {
        return std::make_shared<FileIO>();
    }
    
    return nullptr;
}

void IO::close() {
    _isOpened = false;
}

size_t IO::read(uint8_t *pBuf, size_t size) {
    if (nullptr == pBuf || size <= 0) {
        return 0;
    }
    
    if (!_isOpened) {
        return 0;
    }
    
    size_t readSize = 0;
    _cond.wait([this, size]() -> bool {
        if (size <= _buffer.readableBytes() || !_isOpened) {
            return true;
        } else {
            return false;
        }
    }, [this, pBuf, size, &readSize]() {
        if (!_isOpened) {
            return;
        }
        _buffer.lock();
        if (size > _buffer.readableBytes()) {
            return;
        }
        
        const char *begin = _buffer.beginRead();
        if (begin) {
            readSize = size;
            memcpy(pBuf, begin, readSize);
            _buffer.retrieve(readSize);
        }
        _buffer.unlock();
    });
    
    if (readSize > 0) {
        _readPos += readSize;
    }
    
    return readSize;
}
