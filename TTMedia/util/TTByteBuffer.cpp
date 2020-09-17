//
//  TTByteBuffer.cpp
//  TTMedia
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include <string>

#include "TTMutex.hpp"
#include "TTByteBuffer.hpp"

using namespace TT;

const char ByteBuffer::kCRLF[] = { "\r\n" };

ByteBuffer::ByteBuffer(size_t capacity)
: _mutex(PTHREAD_MUTEX_INITIALIZER)
, capacity_(capacity)
, readIndex_(0)
, writeIndex_(0)
{
}

bool ByteBuffer::lock() {
    pthread_mutex_lock(&_mutex);
    return true;
}

bool ByteBuffer::unlock() {
    pthread_mutex_unlock(&_mutex);
    return true;
}

const char *ByteBuffer::find(const char *sub) {
    size_t size = strlen(sub);
    if (size > readableBytes()) {
        return NULL;
    } else if (strncmp(sub, beginRead(), size) == 0) {
        return beginRead();
    }
    
    const char *res = std::search(beginRead(), beginWrite(), sub, sub + size);
    return res == beginWrite() ? NULL : res;
}

bool ByteBuffer::beginCRLF() {
    if (readableBytes() > 2) {
        char *begin = beginRead();
        if (strncmp(begin, kCRLF, 2) == 0) {
            return true;
        }
    }
    return false;
}

const char *ByteBuffer::findCRLF()
{
    return findCRLF(beginRead());
}

const char *ByteBuffer::findCRLF(char *start)
{
    return find(kCRLF);
}

void ByteBuffer::skipSpace() {
    while (readableBytes()) {
        if (*beginRead() == ' ') {
            retrieve(1);
        } else {
            break;
        }
    }
}

void ByteBuffer::skipCRLF() {
    const char *crlf = find(kCRLF);
    int n = static_cast<int>(crlf - beginRead());
    n += strlen(kCRLF);
    retrieve(n);
}

bool ByteBuffer::getLine(std::string &line) {
    const char *begin = beginRead();
    const char *end = findCRLF();
    if (end == NULL) return false;
    line.assign(begin, end - begin + 2);
    retrieve(line.size());
    return true;
}

int ByteBuffer::readInt() {
    int n = 0;
    while (readableBytes()) {
        char ch = *beginRead();
        if (ch >= '0' && ch <= '9') {
            n *= 10;
            n += ch - '0';
            retrieve(1);
        } else {
            break;
        }
    }
    return n;
}

void ByteBuffer::clear() {
    Mutex m(&_mutex);
    readIndex_ = writeIndex_ = 0;
}

size_t ByteBuffer::append(const char *data, size_t n)
{
    Mutex m(&_mutex);
    if (expendWriteable(n)) {
        std::copy(data, data + n, beginWrite());
        writeIndex_ += n;
    } else {
        n = 0;
    }
    return n;
}

size_t ByteBuffer::appendBuffer(ByteBuffer &buffer)
{
    buffer.lock();
    char *data = buffer.beginRead();
    size_t size = buffer.readableBytes();
    size = append(data, size);
    buffer.unlock();
    return size;
}

bool ByteBuffer::ensureWriteable(size_t n) {
    Mutex m(&_mutex);
    return expendWriteable(n);
}

std::string ByteBuffer::toString() {
    Mutex m(&_mutex);
    return std::string(beginRead(), readableBytes());
}

bool ByteBuffer::expendWriteable(size_t n) {
    if (n > writeableBytes()) {
        expend(n);
        if (n > writeableBytes()) {
            return false;
        }
    }
    return true;
}

void ByteBuffer::expend(size_t size) {
    if (canExpand(size)) {
        if (size > writeableBytes() + prependableBytes()) {
            buffer_.resize(writeIndex_ + size);
        } else {
            size_t readable = readableBytes();
            std::copy(beginRead(), beginWrite(), begin());
            readIndex_ = 0;
            writeIndex_ = readIndex_+ readable;
        }
    }
}

bool ByteBuffer::canExpand(size_t size) {
    if (size > writeableBytes() + prependableBytes()) {
        if (size + readableBytes() > capacity_) {
            return false;
        }
    }
    return true;
}

