//
//  TTClip.hpp
//  TTPlayerExample
//
//  Created by liang on 4/5/19.
//  Copyright © 2019年 tina. All rights reserved.
//

#ifndef TTClip_hpp
#define TTClip_hpp

#include <vector>
#include "TTDef.h"
#include "TTMaterial.hpp"

namespace TT {

class Clip {
public:
    Clip();
    ~Clip();
    
    TT_PROPERTY_DEF(TT_SP(Material), material, nullptr);
    TT_PROPERTY_DEF(int, srcStartIndex, 0);
    TT_PROPERTY_DEF(int, srcEndIndex, 0);
    
    TT_PROPERTY_DEF(int, dstStartIndex, 0);
    TT_PROPERTY_DEF(int, dstEndIndex, 0);
    
private:
};
}

#endif /* TTClip_hpp */
