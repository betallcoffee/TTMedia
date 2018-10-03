//
//  TTFileIO.cpp
//  TTPlayerExample
//
//  Created by liang on 9/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTFileIO.hpp"

using namespace TT;

FileIO::FileIO()
{
    
}

FileIO::~FileIO() {
    
}

bool FileIO::open(std::shared_ptr<URL> url, uint64_t offset, int flag) {
    return false;
}

void FileIO::close() {
    
}

size_t FileIO::size() {
    return 0;
}

size_t FileIO::read(uint8_t *pBuf, size_t size) {
    return 0;
}

size_t FileIO::readAt(uint8_t *pBuf, size_t size, uint64_t pos) {
    return 0;
}

size_t FileIO::write(const uint8_t *pBuf, size_t size) {
    return 0;
}

bool FileIO::seek(uint64_t pos) {
    return false;
}

int64_t FileIO::length() {
    return 0;
}

int64_t FileIO::readPos() {
    return 0;
}

int64_t FileIO::downPos() {
    return 0;
}

int64_t FileIO::speed() {
    return 0;
}
