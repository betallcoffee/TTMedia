//
//  TTMP4Parser.hpp
//  TTMedia
//
//  Created by liang on 24/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTMP4Parser_hpp
#define TTMP4Parser_hpp


#include "TTDef.h"
#include "TTBitStream.hpp"

#include "TTMP4Box.hpp"
#include "TTMP4.h"

/* Big Endian */

namespace TT {
class MP4Parser {
public:
    MP4Parser(TT_SP(BitStream) bitStream);
    ~MP4Parser();
    
    bool parseBox();
    
private:
    bool createBox(uint64_t size, uint32_t type);
    bool createFtyp(uint64_t size, uint32_t type);
    bool createMoov(uint64_t size, uint32_t type);
    
    TT_PROPERTY_DEF_READONLY(TT_SP(BitStream), bitStream, nullptr);
    
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxFtyp), ftyp, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxMoov), moov, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxMdat), mdat, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxMeta), meta, nullptr);
};

}

#endif /* TTMP4Parser_hpp */
