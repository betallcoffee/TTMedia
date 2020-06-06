//
//  TTBilateralFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 3/2/2020.
//  Copyright © 2020 tina. All rights reserved.
// 双边滤波

#ifndef TTBilateralFilter_hpp
#define TTBilateralFilter_hpp

#include "TTGaussianBlurFilter.hpp"

namespace TT {
class BilateraFilter : public GaussianBlurFilter {
public:
    BilateraFilter();
    
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
