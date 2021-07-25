//
//  TTMP4Parser.cpp
//  TTMedia
//
//  Created by liang on 24/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTDef.h"

#include "TTMP4Parser.hpp"
#include "TTMP4Private.h"

using namespace TT;

MP4Parser::MP4Parser(TT_SP(BitStream) bitStream) :
 _bitStream(bitStream)
{
    
}

MP4Parser::~MP4Parser()
{
    
}

bool MP4Parser::parseBox()
{
    if (_bitStream == nullptr || _bitStream->isEof()) {
        return false;
    }
    
    char typeFourcc[5];
    char uuid[16];
    
    uint32_t size = _bitStream->readUInt32();
    uint32_t type = _bitStream->readUInt32();
    
    MP4Box::FourcToStr(type, typeFourcc, sizeof(typeFourcc));
    LOG(INFO) << "type: " << typeFourcc << " size: " << size ;
    
    bool result = false;
    result = createBox(size, type);
    
    if (false) {
        result = _bitStream->skipSize(size - MP4Box::headSize());
        LOG(INFO) << "skip result: " << result;
    }
    
    return true;
}

bool MP4Parser::createBox(uint64_t size, uint32_t type)
{
    switch (type) {
        case MP4_BOX_TYPE_ftyp:
            return createFtyp(size, type);
            break;
        case MP4_BOX_TYPE_moov:
            return createMoov(size, type);
            break;
        default:
            break;
    }
    return false;
}

bool MP4Parser::createFtyp(uint64_t size, uint32_t type)
{
    _ftyp = TT_MK_SP(MP4BoxFtyp)(size, type);
    _ftyp->parseData(_bitStream);
    return true;
}

bool MP4Parser::createMoov(uint64_t size, uint32_t type)
{
    _moov = TT_MK_SP(MP4BoxMoov)(size, type);
    _moov->parseData(_bitStream);
    return true;
}

