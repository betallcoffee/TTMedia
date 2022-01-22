//
//  TTMP4BoxFactory.hpp
//  TTMedia
//
//  Created by liang on 16/1/2022.
//  Copyright © 2022 tina. All rights reserved.
//

#ifndef TTMP4BoxFactory_hpp
#define TTMP4BoxFactory_hpp

#include "TTDef.h"
#include "TTMP4Box.hpp"
#include "TTBitStream.hpp"

namespace TT {

class MP4BoxFactory {
public:
    static TT_SP(MP4Box) createBox(TT_SP(BitStream) bitStream);
    static TT_SP(MP4Box) createBox(uint64_t size, uint32_t type);
};


}

#endif /* TTMP4BoxFactory_hpp */
