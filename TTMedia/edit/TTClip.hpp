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

namespace TT {
    class Media;
    
    class Clip {
    public:
        Clip();
        ~Clip();
        
        TT_PROPERTY_DEF(TT_SP(Media), media);
        TT_PROPERTY_DEF(int, srcStartIndex);
        TT_PROPERTY_DEF(int, srcEndIndex);
        
        TT_PROPERTY_DEF(int, dstStartIndex);
        TT_PROPERTY_DEF(int, dstEndIndex);
        
    private:
    };
}

#endif /* TTClip_hpp */
