//
//  TTMP4Parser.hpp
//  TTMedia
//
//  Created by liang on 24/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTMP4Parser_hpp
#define TTMP4Parser_hpp

#include <stack>

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
    
    bool parse();
    
private:
    void saveBox(TT_SP(MP4Box) box);
    
    TT_PROPERTY_DEF_READONLY(bool, isEof, false);
    TT_PROPERTY_DEF_READONLY(TT_SP(BitStream), bitStream, nullptr);
    
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxFtyp), ftyp, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxMoov), moov, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxMdat), mdat, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4BoxMeta), meta, nullptr);
    
private:
    std::stack<TT_SP(MP4Box)> boxs;
};

}

#endif /* TTMP4Parser_hpp */
