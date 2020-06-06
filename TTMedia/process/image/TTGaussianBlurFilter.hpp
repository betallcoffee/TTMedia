//
//  TTGaussianBlurFilter.hpp
//  TTMedia
//
//  Created by liang on 25/1/2020.
//  Copyright © 2020 tina. All rights reserved.
// 高斯模糊

#ifndef TTGaussianBlurFilter_hpp
#define TTGaussianBlurFilter_hpp

#include <string>
#include "TTTwoPassFilter.hpp"

namespace TT {
class GaussianBlurFilter : public TwoPassFilter {
public:
    GaussianBlurFilter();
    
    void setTexelSpacingMultiplier(float texelSpacing);
    void setBlurRadius(float radius);
    
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
    void createVertexShader();
    void createFragmentShader();
    
private:
    std::string _vertexShader;
    std::string _fragmentShader;
    
    GLint _verticalPassTexelWidthOffsetUniform = 0;
    GLint _verticalPassTexelHeightOffsetUniform = 0;
    GLint _horizontalPassTexelWidthOffsetUniform = 0;
    GLint _horizontalPassTexelHeightOffsetUniform = 0;
    
    GLfloat _verticalPassTexelWidthOffset = 0;
    GLfloat _verticalPassTexelHeightOffset = 0;
    GLfloat _horizontalPassTexelWidthOffset = 0;
    GLfloat _horizontalPassTexelHeightOffset = 0;
    
    
    /** A multiplier for the spacing between texels, ranging from 0.0 on up, with a default of 1.0. Adjusting this may slightly increase the blur strength, but will introduce artifacts in the result.
     */
    float _texelSpacingMultiplier = 1.0;
    
    float _verticalTexelSpacing = 1.0;
    float _horizontalTexelSpacing = 1.0;

    /** A radius to use for the blur, with a default of 4.0. This adjusts the sigma variable in the Gaussian distribution function.
    */
    float _blurRadius = 4.0;
    /** A radius in pixels to use for the blur, with a default of 2.0. This adjusts the sigma variable in the Gaussian distribution function.
     */
    float _blurRadiusInPixels = 0;
    
    float _sigma = 10.0;
};
}

#endif /* TTGaussianBlurFilter_hpp */
