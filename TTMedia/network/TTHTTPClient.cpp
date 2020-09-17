//
//  TTHTTPClient.cpp
//  TTMedia
//
//  Created by liang on 26/11/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"
#include "TTHTTPClient.hpp"

using namespace TT;

static const char *kCRLF = "\r\n";
static const char kColon = ':';
static const char *kHTTP = "HTTP/1.";

HTTPClient::HTTPClient()
: _buffer(kMaxBufferSize)
, _mutex(PTHREAD_MUTEX_INITIALIZER)
, _cond(&_mutex)
, _isCanceled(false)
{
    resetResponse();
}

HTTPClient::~HTTPClient() {
    
}
//GET /%s HTTP/1.1\r\n%sHost: %s:%d\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n
void HTTPClient::Get(std::shared_ptr<URL> url,
                     const HeaderMap &headers,
                     DataRecivedCallback dataCallback,
                     ErrorCallback errorCallback) {
    std::string request = "GET ";
    request += url->path();
    request += " HTTP/1.1";
    request += kCRLF;
    
    request += "Connection: Close";
    request += kCRLF;
    
    request += "Host: ";
    request += url->host();
    request += kCRLF;
    
    request += "User-Agent: TTPlayer";
    request += kCRLF;
    
    request += "Accept: */*";
    request += kCRLF;
    
    request += kCRLF;
    
    resetResponse();
    
    _socket = std::make_shared<TCPSocket>();
    if (!_socket->init()){
        LOG(ERROR) << "Init socket failed:" << url->string();
        if (errorCallback) {
            errorCallback();
        }
        return;
    }
    
    _isCanceled = false;
    if (_socket->connect(url)) {
        size_t writeSize = _socket->write(reinterpret_cast<const uint8_t *>(request.c_str()), request.size());
        _parseStatus = kParseFirstLine;
        if (writeSize == request.size()) {
            do {
                if (_isCanceled) {
                    break;
                }
                
                int ret = _socket->read(reinterpret_cast<uint8_t *>(_bytes), kMaxBytesSize);
                if (ret < 0) {
                    break;
                }
                
                if (!process(_bytes, ret, dataCallback, errorCallback)) {
                    break;
                }
                
            } while (true);
            
            if (errorCallback) {
                errorCallback();
            }
        }
    }
}

void HTTPClient::cancel() {
    _isCanceled = true;
}

void HTTPClient::skipToNext() {
    const char *begin = _buffer.beginRead();
    while (*begin == ' ') {
        _buffer.retrieve(1);
    }
}

void HTTPClient::resetResponse() {
    _httpVer = "";
    _statusCode = 0;
    _phase = "";
    
    _isClose = false;
    _isChunked = false;
    _contentType = "";
    _contentLength = 0;
}

bool HTTPClient::process(const char *data, size_t size, DataRecivedCallback dataCallback, ErrorCallback errorCallback) {
    bool ret = true;
    size_t n = 0;
    do {
        if (_isCanceled) {
            ret = false;
            break;
        }
        
        n = _buffer.append(_bytes, size);
        if (kParseFirstLine == _parseStatus) {
            const char *lineEnd = _buffer.findCRLF();
            if (lineEnd != nullptr) {
                if (!parserFirstLine()) {
                    ret = false;
                    break;
                }
                _parseStatus = kParseHeader;
            }
        } else if (kParseHeader == _parseStatus) {
            const char *lineEnd = _buffer.findCRLF();
            if (lineEnd != nullptr) {
                if (!parserHeaders()) {
                    ret = false;
                    break;
                }
            }
        } else if (kParseBody == _parseStatus) {
            if (dataCallback) {
                dataCallback(_buffer);
            }
        }
    } while(n != size);
    
    return ret;
}

bool HTTPClient::parserFirstLine() {
    bool ret = false;
    _buffer.lock();
    const char *begin = _buffer.beginRead();
    size_t len = std::strlen(kHTTP);
    if (std::strncmp(begin, kHTTP, len) == 0) {
        _httpVer.assign(begin + len - 2, 3);
        const char *space = _buffer.find(" ");
        if (space) {
            _buffer.retrieve(space - begin);
            _buffer.skipSpace();
            _statusCode = _buffer.readInt();
            
            _buffer.skipSpace();
            _phase.assign(_buffer.beginRead(), _buffer.findCRLF() - _buffer.beginRead());
            
            _buffer.skipCRLF();
            ret = true;
        }
    }

    _buffer.unlock();
    return ret;
}

bool HTTPClient::parserHeaders() {
    bool ret = true;
    _buffer.lock();
    if (_buffer.beginCRLF()) {
        _buffer.skipCRLF();
        _parseStatus = kParseBody;
    } else {
        const char *lineBegin = _buffer.beginRead();
        const char *lineEnd = _buffer.findCRLF();
        const char *colonPos = std::find(lineBegin, lineEnd, kColon);
        if (lineBegin == colonPos) {
            ret = false;
        } else {
            std::string key(_buffer.beginRead(), colonPos - _buffer.beginRead());
            _buffer.retrieve(key.size() + 1);
            _buffer.skipSpace();
            
            std::string value(_buffer.beginRead(), lineEnd - _buffer.beginRead());
            _buffer.skipCRLF();
            
            _responseHeaders[key] = value;
            if (key == "Connection") {
                if (value == "close") {
                    _isClose = true;
                }
            } else if (key == "Transfer-Encoding") {
                if (value == "chunked") {
                    _isChunked = true;
                } else {
                    _isChunked = false;
                }
            } else if (key == "Content-Type") {
                _contentType = value;
            } else if (key == "Content-Length") {
                _contentLength = std::stol(value);
            }
        }
    }
    _buffer.unlock();
    return ret;
}
