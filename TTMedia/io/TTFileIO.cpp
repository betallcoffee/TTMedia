//
//  TTFileIO.cpp
//  TTMedia
//
//  Created by liang on 9/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include <sys/stat.h>

#include "TTFileIO.hpp"

using namespace TT;

FileIO::FileIO()
{
    
}

FileIO::~FileIO() {
    
}

bool FileIO::canOpen(std::shared_ptr<URL> url) {
    const std::string &scheme = url->scheme();
    if (scheme == "file") {
        return true;
    }
    
    return false;
}

bool FileIO::open(std::shared_ptr<URL> url, uint64_t offset, int flag) {
    _fd = fopen(url->path().c_str(), "r");
    if (_fd == nullptr) {
        return false;
    }
    _url = url;
    
    struct stat buf;
    int ret = stat(_url->path().c_str(), &buf);
    if (ret) {
        _fileSize = 0;
    } else {
        _fileSize = buf.st_size;
    }
    _canReadPos = _fileSize;
    
    seek(0);
    
    _isOpened = true;
    return true;
}

void FileIO::close() {
    if (_fd != nullptr) {
        fclose(_fd);
        _fd = nullptr;
    }
    _isOpened = false;
}

size_t FileIO::size() {
    return _fileSize;
}

size_t FileIO::read(uint8_t *pBuf, size_t size) {
    return fread(pBuf, 1, size, _fd);
}

size_t FileIO::readAt(uint8_t *pBuf, size_t size, uint64_t pos) {
    if (seek(pos)) {
        return read(pBuf, size);
    }
    return 0;
}

size_t FileIO::write(const uint8_t *pBuf, size_t size) {
    return 0;
}

bool FileIO::seek(uint64_t pos) {
    int ret = fseek(_fd, pos, SEEK_SET);
    if (ret) {
        return false;
    }
    return true;
}

int64_t FileIO::readPos() {
    int64_t ret = ftell(_fd);
    return ret;
}

int64_t FileIO::speed() {
    return 0;
}
