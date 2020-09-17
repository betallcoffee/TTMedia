//
//  TTBilateralFilter.hpp
//  TTMedia
//
//  Created by liang on 3/2/2020.
//  Copyright © 2020 tina. All rights reserved.
// 双边滤波

#ifndef TTBilateralFilter_hpp
#define TTBilateralFilter_hpp

#include "TTGaussianBlurFilter.hpp"

namespace TT {

/**
 * 考虑到了颜色的差异，它的像素点取值也是周边像素点的加权平均，
 * 而且权重也是高斯函数。不同的是，这个权重不仅与像素间距离有关，
 * 还与像素值本身的差异有关，具体讲是，像素值差异越小，权重越大，也是这个特性让它具有了保持边缘的特性
 */
class BilateralFilter : public GaussianBlurFilter {
public:
    BilateralFilter();
    
protected:
    virtual const GLchar* vertexShader();
    virtual const GLchar* vertexShader2();
    
    virtual const GLchar* fragmentShader();
    virtual const GLchar* fragmentShader2();
    
    virtual void getUniformLocations();
    virtual void resolveUniformLocations();
    
    virtual void getUniformLocations2();
    virtual void resolveUniformLocations2();
    
private:
    // gl shader uniform
    GLint _firstDistanceNormalizationFactorUniform = 0;
    GLint _secondDistanceNormalizationFactorUniform = 0;
    
    // A normalization factor for the distance between central color and sample color.
    float _distanceNormalizationFactor = 2;
    
};
}

#endif /* TTBilateralFilter_hpp */
