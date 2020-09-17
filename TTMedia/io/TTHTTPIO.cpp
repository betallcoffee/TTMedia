//
//  TTHTTPIO.cpp
//  TTMedia
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
    if (_isOpened) {
        return false;
    }
    
    _url = url;
    _isOpened = true;
    std::shared_ptr<Message> openMessage = std::make_shared<Message>(kIOMessageTypeOpen);
    openMessage->setMessageHandle([this, url](std::shared_ptr<Message> msg) {
        _readPos = 0;
        _downPos = 0;
        _buffer.clear();
        _client.Get(url, _headers, std::bind(&HTTPIO::onDataRecived, this, std::placeholders::_1), nullptr);
    });
    _messageLoop.postMessage(openMessage);
    
    _cond.wait([this]() -> bool {
        if (!_isOpened) {
            return true;
        } else if (_buffer.readableBytes() > 0) {
            return true;
        } else {
            return false;
        }
    }, [this]() {
        
    });
    
    if (!_isOpened) {
        return false;
    } else if (_buffer.readableBytes() > 0) {
        return true;
    } else {
        return false;
    }
}

void HTTPIO::close() {
    IO::close();
    _client.cancel();
}

size_t HTTPIO::size() {
    if (!_isOpened) {
        return 0;
    }
    return static_cast<size_t>(_client.contentLength());
}

size_t HTTPIO::readAt(uint8_t *pBuf, size_t size, uint64_t pos) {
    return 0;
}

size_t HTTPIO::write(const uint8_t *pBuf, size_t size) {
    return 0;
}

bool HTTPIO::seek(uint64_t pos) {
    _buffer.lock();
    if (pos < _downPos) {
        size_t offset = pos - _readPos;
        _buffer.retrieve(offset);
    }
    _buffer.unlock();
    return false;
}

int64_t HTTPIO::speed() {
    return 0;
}

void HTTPIO::onDataRecived(ByteBuffer &data) {
    while (true) {
        if (!_isOpened) {
            break;
        }
        
        bool isEnd;
        _cond.notify([this, &data, &isEnd]() {
            size_t size = data.readableBytes();
            size_t n = _buffer.appendBuffer(data);
            _downPos += n;
            data.retrieve(n);
            if (n == size) {
                isEnd = true;
            } else {
                isEnd = false;
            }
        });
        
        if (isEnd) {
            break;
        }
    }
}


