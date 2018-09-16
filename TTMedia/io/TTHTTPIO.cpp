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
: _mutex(PTHREAD_MUTEX_INITIALIZER)
, _cond(&_mutex)
, _buffer(10 * 1024 * 1024)
{
    _messageLoop.setMessageHandle(std::bind(&HTTPIO::handleMessage, this, std::placeholders::_1));
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
    _url = url;
    _messageLoop.postMessage(kIOMessageTypeOpen);
    return true;
}

void HTTPIO::close() {
    _messageLoop.stop();
}

size_t HTTPIO::read(uint8_t *pBuf, size_t size) {
    if (nullptr == pBuf || size <= 0) {
        return 0;
    }
    
    size_t readSize = 0;
    _cond.wait([this, size](){
        size_t readableSize = _buffer.readableBytes();
        if (readableSize >= size) {
            return true;
        } else {
            return false;
        }
    }, [this, pBuf, size, &readSize](){
        const char *begin = _buffer.beginRead();
        if (begin) {
            size_t readableSize = _buffer.readableBytes();
            readSize = size <= readableSize ? size : readableSize;
            memcpy(pBuf, begin, readSize);
            _buffer.retrieve(readSize);
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
    _cond.notify([this, &data](){
        size_t size = _buffer.appendBuffer(data);
        data.retrieve(size);
    });
}

void HTTPIO::handleMessage(std::shared_ptr<Message> message) {
    if (kIOMessageTypeOpen == message->code()) {
        _client.Get(_url, _headers, std::bind(&HTTPIO::onDataRecived, this, std::placeholders::_1), nullptr);
    }
}





