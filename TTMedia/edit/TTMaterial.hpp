//
//  TTMaterial.hpp
//  TTPlayerExample
//
//  Created by liang on 11/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTMaterial_hpp
#define TTMaterial_hpp

#include "TTdef.h"
#include "TTURL.hpp"
#include "TTFrame.hpp"

namespace TT {
    
    typedef enum class MaterialType {
        kNone,
        kVideo,
        kAudio,
    } eMaterialType;
    
    class Material {
    public:
        Material(eMaterialType type, std::shared_ptr<URL> url = nullptr);
        ~Material();
        
        int tag() { return _tag; }
        void setTag(int tag) { _tag = tag; }
        
        eMaterialType type() { return _type; }
        
        virtual bool open() = 0;
        virtual bool close() = 0;
        virtual void save(std::shared_ptr<URL> url) = 0;
        virtual int frameCount() = 0;
        virtual std::shared_ptr<Frame> frame(int index) = 0;
        virtual bool loadMore() = 0;
        
        int startIndex;
        int endIndex;
        
        TT_PROPERTY_DEF_READONLY(bool, isOpen);
        TT_PROPERTY_DEF_READONLY(bool, isEnd);
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<URL>, url);
    protected:
        int _tag = 0;
        eMaterialType _type;
    };
}

#endif /* TTMaterial_hpp */
