//
//  TTIO.cpp
//  TTPlayerExample
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
