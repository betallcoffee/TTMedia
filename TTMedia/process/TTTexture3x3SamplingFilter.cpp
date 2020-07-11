//
//  TTTexture3x3SamplingFilter.cpp
//  TTMeida
//
//  Created by liang on 14/6/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTTexture3x3SamplingFilter.hpp"

using namespace TT;

static const GLchar *const kTexture3x3SamplingVertexShader = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 texcoord;
 
 uniform float texelWidth;
 uniform float texelHeight;
 
 varying vec2 v_textureCoordinate;
 varying vec2 v_leftTextureCoordinate;
 varying vec2 v_rightTextureCoordinate;
 
 varying vec2 v_topTextureCoordinate;
 varying vec2 v_topLeftTextureCoordinate;
 varying vec2 v_topRightTextureCoordinate;
 
 varying vec2 v_bottomTextureCoordinate;
 varying vec2 v_bottomLeftTextureCoordinate;
 varying vec2 v_bottomRightTextureCoordinate;
 
 void main()
 {
     gl_Position = position;
    
     vec2 widthStep = vec2(texelWidth, 0.0);
     vec2 heightStep = vec2(0.0, texelHeight);
     vec2 widthHeightStep = vec2(texelWidth, texelHeight);
     vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);
     
     v_textureCoordinate = texcoord.xy;
     v_leftTextureCoordinate = texcoord.xy - widthStep;
     v_rightTextureCoordinate = texcoord.xy + widthStep;
     
     v_topTextureCoordinate = texcoord.xy - heightStep;
     v_topLeftTextureCoordinate = texcoord.xy - widthHeightStep;
     v_topRightTextureCoordinate = texcoord.xy + widthNegativeHeightStep;
     
     v_bottomTextureCoordinate = texcoord.xy + heightStep;
     v_bottomLeftTextureCoordinate = texcoord.xy - widthNegativeHeightStep;
     v_bottomRightTextureCoordinate = texcoord.xy + widthHeightStep;
 }
 );

Texture3x3SamplingFilter::Texture3x3SamplingFilter()
{
    
}

Texture3x3SamplingFilter::~Texture3x3SamplingFilter()
{
    
}

void Texture3x3SamplingFilter::setupFilterSize()
{
    if (_width > 0) {
        _texelWidth = 1.0 / _width;
    }
    
    if (_height > 0) {
        _texelHeight = 1.0 / _height;
    }
}

const GLchar *Texture3x3SamplingFilter::vertexShader()
{
    return kTexture3x3SamplingVertexShader;
}

void Texture3x3SamplingFilter::getUniformLocations()
{
    Filter::getUniformLocations();
    _texelWidthUniform = _program.getUniformLocation("texelWidth");
    _texelHeightUniform = _program.getUniformLocation("texelHeight");
}

void Texture3x3SamplingFilter::resolveUniformLocations()
{
    Filter::resolveUniformLocations();
    glUniform1f(_texelWidthUniform, _texelWidth);
    glUniform1f(_texelHeightUniform, _texelHeight);
}
