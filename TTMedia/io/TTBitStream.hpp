//
//  TTBitStream.hpp
//  TTMediaExample
//
//  Created by liang on 11/2/2021.
//  Copyright © 2021 tina. All rights reserved.
//

#ifndef TTBitStream_hpp
#define TTBitStream_hpp

#include "TTDef.h"
#include "TTIO.hpp"

namespace TT {

class BitStream
{
public:
    BitStream(TT_SP(IO) io);
    ~BitStream();
    
    uint32_t readInt32(uint32_t nBits);
    uint64_t readInt64(uint32_t nBits);
    
    float readFloat();
    double readDouble();
    size_t readData(uint8_t *pBuf, size_t size);
    
    uint8_t readUInt8();
    uint16_t readUInt16();
    uint32_t readUInt24();
    uint32_t readUInt32();
    uint64_t readUInt64();
    uint32_t readUInt32Le();
    uint16_t readUInt16Le();
    
    TT_PROPERTY_DEF(TT_SP(IO), io, nullptr)
    TT_PROPERTY_DEF(uint8_t, currentByte, 0)
    TT_PROPERTY_DEF(int8_t, nBitsOfByte, 0)
};

}

#endif /* TTBitStream_hpp */
