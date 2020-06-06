//
//  TTColorMatrixFilter.hpp
//  TTMedia
//
//  Created by liang on 21/1/20.
//  Copyright © 2020年 tina. All rights reserved.
// 颜色矩阵

#ifndef TTColorMatrixFilter_hpp
#define TTColorMatrixFilter_hpp

/**
 可通过用 3×3 矩阵乘以这些颜色矢量将线性变换（旋转和缩放等）应用到颜色矢量中。但是，您不能使用 3×3 矩阵进行平移（非线性）。如果在每个颜色矢量中再添加一个虚拟的第 4 坐标（例如，数字 1），则可使用 4×4 矩阵应用任何组合形式的线性变换和平移。由线性变换组成的后跟平移的变换称为仿射变换。
 */

#include "TTFilter.hpp"
#include "TTFilterCommon.h"

namespace TT {
    class ColorMatrixFilter : public Filter {
    public:
        ColorMatrixFilter();
        void setColorMatrix(GPUMatrix4x4 &colorMatrix);
        void setIntensity(float intensity);
        
    private:
        virtual const GLchar* fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    protected:
        GLfloat _colorMatrixUniform = 0;
        GLfloat _intensityUniform = 0;
        GPUMatrix4x4 _colorMatrix;
        float _intensity = 1.0;
    };
}

#endif /* TTColorMatrixFilter_hpp */
