//
//  TTMaterial.hpp
//  TTMedia
//
//  Created by liang on 22/8/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTMaterial_hpp
#define TTMaterial_hpp

#include "TTDef.h"

#include "TTURL.hpp"
#include "TTFrame.hpp"

namespace TT {

class Material {
public:
    Material() {};
    ~Material() {};
    
    virtual bool open() { return false; }
    virtual bool open(TT_SP(URL) url) { return false; }
    virtual bool close() { return false; }
    
    virtual void save(TT_SP(URL) url) {}
    virtual bool loadMore() { return false; }
    
    virtual int frameCount() { return 1; }
    virtual TT_SP(Frame) frameAt(int index) { return frame(); }
    
    TT_PROPERTY_DEF_READONLY(bool, isOpen, false);
    TT_PROPERTY_DEF_READONLY(bool, isEnd, true);
    
    TT_PROPERTY_DEF(TT_SP(URL), url, nullptr);
    TT_PROPERTY_DEF_READONLY(TT_SP(Frame), frame, nullptr);
    
    TT_PROPERTY_DEF_READONLY(size_t, width, 0);
    TT_PROPERTY_DEF_READONLY(size_t, height, 0);
    
    
};

}


#endif /* TTMaterial_hpp */
