//
//  TTFilterTexture.hpp
//  TTMedia
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//
// 上传 RGB 图片数据到 gpu 生成纹理到 GL_TEXTURE1，不需要 draw

#ifndef TTFilterTexture_hpp
#define TTFilterTexture_hpp

#include "TTFilter.hpp"

namespace TT {
    class FilterTexture : public Filter {
    public:
        FilterTexture();
        virtual ~FilterTexture();
        
        virtual void process(int64_t timestamp);
    };
}

#endif /* TTFilterTexture_hpp */
