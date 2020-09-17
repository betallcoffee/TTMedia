//
//  TTFilterFrame.hpp
//  TTMedia
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//
// 接收 Frame 类型的图片数据，由子类重写 updateTexture() 方法，将不同格式的图片数据上传到 gpu 中，draw 生成 rgb 纹理到 GL_TEXTURE1

#ifndef TTFilterFrame_hpp
#define TTFilterFrame_hpp

#include "TTFrame.hpp"
#include "TTPacket.hpp"
#include "TTFilter.hpp"

namespace TT {
    class FilterFrame : public Filter {
    public:
        FilterFrame();
        virtual ~FilterFrame();
        
        virtual void processFrame(std::shared_ptr<Frame> frame);
        
    protected:
        std::shared_ptr<Frame> _frame;
    };
}

#endif /* TTFilterFrame_hpp */
