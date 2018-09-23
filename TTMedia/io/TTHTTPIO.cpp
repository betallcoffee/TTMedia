//
//  TTHTTPIO.cpp
//  TTPlayerExample
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTHTTPIO.hpp"

using namespace TT;

typedef enum {
    kIOMessageTypeOpen,
} IOMessageType;

HTTPIO::HTTPIO()
: _messageLoop("HTTPIO")
, _cond(&_mutex)
, _buffer(10 * 1024 * 1024)
{
}

HTTPIO::~HTTPIO() {
    _messageLoop.stop();
}

bool HTTPIO::canOpen(std::shared_ptr<URL> url) {
    const std::string &scheme = url->scheme();
    if (scheme == "http") {
        return true;
    }
    
    return false;
}

bool HTTPIO::open(std::shared_ptr<URL> url, uint64_t offset, int flag) {
    close();
    _url = url;
    std::shared_ptr<Message> openMessage = std::make_shared<Message>(kIOMessageTypeOpen);
    openMessage->setMessageHandle([this, url](std::shared_ptr<Message> msg) {
        _isOpened = true;
        _client.Get(url, _headers, std::bind(&HTTPIO::onDataRecived, this, std::placeholders::_1), nullptr);
    });
    _messageLoop.postMessage(openMessage);
    return true;
}

void HTTPIO::close() {
    _isOpened = false;
    _client.cancel();
}

size_t HTTPIO::read(uint8_t *pBuf, size_t size) {
    if (nullptr == pBuf || size <= 0) {
        return 0;
    }
    
    if (!_isOpened) {
        return 0;
    }
    
    size_t readSize = 0;
    _cond.wait([this, size](){
        if (!_isOpened) {
            return true;
        }
        size_t readableSize = _buffer.readableBytes();
        if (readableSize >= size) {
            return true;
        } else {
            return false;
        }
    }, [this, pBuf, size, &readSize](){
        if (_isOpened) {
            const char *begin = _buffer.beginRead();
            if (begin) {
                size_t readableSize = _buffer.readableBytes();
                readSize = size <= readableSize ? size : readableSize;
                memcpy(pBuf, begin, readSize);
                _buffer.retrieve(readSize);
            }
        }
    });
    
    return readSize;
}

size_t HTTPIO::readAt(uint8_t *pBuf, size_t size, uint64_t pos) {
    return 0;
}

size_t HTTPIO::write(const uint8_t *pBuf, size_t size) {
    return 0;
}

bool HTTPIO::seek(uint64_t pos) {
    return false;
}

int64_t HTTPIO::length() {
    return 0;
}

int64_t HTTPIO::readPos() {
    return 0;
}

int64_t HTTPIO::downPos() {
    return 0;
}

int64_t HTTPIO::speed() {
    return 0;
}

void HTTPIO::onDataRecived(ByteBuffer &data) {
    if (!_isOpened) {
        return;
    }
    _cond.notify([this, &data](){
        size_t size = _buffer.appendBuffer(data);
        data.retrieve(size);
    });
}





