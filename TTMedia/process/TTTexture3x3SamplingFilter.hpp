//
//  TTTexture3x3SamplingFilter.hpp
//  TTMeida
//
//  Created by liang on 14/6/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTTexture3x3SamplingFilter_hpp
#define TTTexture3x3SamplingFilter_hpp

#include "TTdef.h"
#include "TTFilter.hpp"

namespace TT {

class Texture3x3SamplingFilter : public Filter {
public:
    Texture3x3SamplingFilter();
    ~Texture3x3SamplingFilter();
    
    void setTexelWidth(float width) { _texelWidth = width; }
    void setTexelHeight(float height) { _texelHeight = height; }
    
private:
    virtual void setupFilterSize() override;
    
    /**
     *返回自定义顶点 shader
     *其中变量约定：
     * attribute vec4 position; // 顶点坐标
     * attribute vec2 texcoord; // 纹理坐标
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
    virtual const GLchar *vertexShader() override;
    
    virtual void getUniformLocations() override;
    virtual void resolveUniformLocations() override;
    
    
private:
    // The texel width and height determines how far out to sample from this texel. By default, this is the normalized width of a pixel, but this can be overridden for different effects.
    float _texelWidth;
    float _texelHeight;
    
    GLint _texelWidthUniform;
    GLint _texelHeightUniform;
};

}

#endif /* TTTexture3x3SamplingFilter_hpp */
