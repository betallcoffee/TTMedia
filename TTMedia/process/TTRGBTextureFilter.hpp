//
//  TTRGBTextureFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 23/8/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTRGBTextureFilter_hpp
#define TTRGBTextureFilter_hpp

#include "TTDef.h"
#include "TTFilterTexture.hpp"
#include "TTFrame.hpp"

namespace TT {

class RGBTextureFilter : public FilterTexture {
public:
    RGBTextureFilter() {};
    ~RGBTextureFilter() {};
    
    virtual void updateTexture() override;
    
    void processFrame(TT_SP(Frame) frame);
    
    TT_PROPERTY_DEF_READONLY(TT_SP(Frame), frame, nullptr);
};

}

#endif /* TTRGBTextureFilter_hpp */
