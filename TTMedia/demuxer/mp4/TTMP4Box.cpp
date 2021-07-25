//
//  TTMP4Box.cpp
//  TTMediaExample
//
//  Created by liang on 20/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTMP4Box.hpp"

using namespace TT;

MP4Box::MP4Box(uint64_t size, uint32_t type)
: _size(size)
, _type(type)
{
    
}

MP4Box::~MP4Box()
{
    
}

bool MP4Box::FourcToStr(uint32_t type, char *buf, size_t size)
{
    uint32_t i;
    int ch;
    
    if (buf == NULL || size < 5)
        return false;
    
    for (i = 0; i < 4; i++, buf++) {
        ch = type >> (8 * (3 - i)) & 0xff;
        if (isprint(ch))
            *buf = ch;
        else
            *buf = '.';
    }
    
    *buf = 0;
    
    return true;
}

bool MP4BoxFtyp::parseData(TT_SP(BitStream) bitStream)
{
    _majorBrand = bitStream->readUInt32();
    char fourcc[5];
    MP4Box::FourcToStr(_majorBrand, fourcc, sizeof(fourcc));
    LOG(INFO) << "majorBrand: " << fourcc;
    _minorVersion = bitStream->readUInt32();
    
    uint64_t remainSize = size() - headSize() - 8;
    _compatibleBrandsCount = remainSize / 4;
    _compatibleBrands = new uint32_t[_compatibleBrandsCount]();
    for (int i = 0; i < _compatibleBrandsCount; i++) {
        _compatibleBrands[i] = bitStream->readUInt32();
        MP4Box::FourcToStr(_compatibleBrands[i], fourcc, sizeof(fourcc));
        LOG(INFO) << "compatibleBrands: " << fourcc;
    }
    
    return true;
}

bool MP4BoxMoov::parseData(TT_SP(BitStream) bitStream)
{
    uint32_t size = bitStream->readUInt32();
    uint32_t type = bitStream->readUInt32();
    
    char fourcc[5];
    MP4Box::FourcToStr(type, fourcc, sizeof(fourcc));
    LOG(INFO) << "type: " << fourcc << " size: " << size ;
    return true;
}
