//
//  TTConvolution3x3Filter.hpp
//  TTMedia
//
//  Created by liang on 19/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

/** Convolution kernel to run against the image

The convolution kernel is a 3x3 matrix of values to apply to the pixel and its 8 surrounding pixels.
The matrix is specified in row-major order, with the top left pixel being one.one and the bottom right three.three
If the values in the matrix don't add up to 1.0, the image could be brightened or darkened.
*/

#ifndef TTConvolution3x3Filter_hpp
#define TTConvolution3x3Filter_hpp

#include "TTTexture3x3SamplingFilter.hpp"

namespace TT {

class Convolution3x3Filter : public Texture3x3SamplingFilter {
public:
    Convolution3x3Filter();
    ~Convolution3x3Filter();
    
    /**
     *返回自定义的片段 shader
     *其中约定 ：
     * varying highp vec2 v_texcoord; // 顶点 shader 传过来的纹理坐标信息
     * uniform sampler2D texture; // 输入纹理
     *
     * varying vec2 v_textureCoordinate;
     * varying vec2 v_leftTextureCoordinate;
     * varying vec2 v_rightTextureCoordinate;
     *
     * varying vec2 v_topTextureCoordinate;
     * varying vec2 v_topLeftTextureCoordinate;
     * varying vec2 v_topRightTextureCoordinate;
     *
     * varying vec2 v_bottomTextureCoordinate;
     * varying vec2 v_bottomLeftTextureCoordinate;
     * varying vec2 v_bottomRightTextureCoordinate;
     */
    virtual const GLchar *fragmentShader() override;
    
    /**
     * shader 中 uniform 变量在每次 process 都会赋值
     */
    virtual void getUniformLocations() override;
    virtual void resolveUniformLocations() override;
    
private:
    GLint _convolutionKernelUniform;
    
    TT_PROPERTY_DEF_NOINIT(GPUMatrix3x3, convolutionKernel);
    
};

}

#endif /* TTConvolution3x3Filter_hpp */
