//
//  TTDirectionalNonmaximumSuppressionFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTDirectionalNonmaximumSuppressionFilter.hpp"

using namespace TT;

static const GLchar* const kDirectionalNonmaximumSuppressionFragmentShader = STRINGIZE
(
 precision mediump float;
 
 varying highp vec2 v_texcoord; // 顶点 shader 传过来的纹理坐标信息
 uniform sampler2D texture; // 输入纹理
 
 uniform highp float texelWidth;
 uniform highp float texelHeight;
 uniform mediump float upperThreshold;
 uniform mediump float lowerThreshold;

 void main()
 {
     vec3 currentGradientAndDirection = texture2D(texture, v_texcoord).rgb;
     vec2 gradientDirection = ((currentGradientAndDirection.gb * 2.0) - 1.0) * vec2(texelWidth, texelHeight);
     
     float firstSampledGradientMagnitude = texture2D(texture, v_texcoord + gradientDirection).r;
     float secondSampledGradientMagnitude = texture2D(texture, v_texcoord - gradientDirection).r;
     
     float multiplier = step(firstSampledGradientMagnitude, currentGradientAndDirection.r);
     multiplier = multiplier * step(secondSampledGradientMagnitude, currentGradientAndDirection.r);
     
     float thresholdCompliance = smoothstep(lowerThreshold, upperThreshold, currentGradientAndDirection.r);
     multiplier = multiplier * thresholdCompliance;
     
     gl_FragColor = vec4(multiplier, multiplier, multiplier, 1.0);
 }
 );

DirectionalNonmaximumSuppressionFilter::DirectionalNonmaximumSuppressionFilter()
{
    
}

DirectionalNonmaximumSuppressionFilter::~DirectionalNonmaximumSuppressionFilter()
{
    
}

void DirectionalNonmaximumSuppressionFilter::setupFilterSize()
{
    if (_width > 0) {
        _texelWidth = 1.0 / _width;
    }
    
    if (_height > 0) {
        _texelHeight = 1.0 / _height;
    }
}

const GLchar *DirectionalNonmaximumSuppressionFilter::fragmentShader()
{
    return kDirectionalNonmaximumSuppressionFragmentShader;
}

void DirectionalNonmaximumSuppressionFilter::getUniformLocations()
{
    Filter::getUniformLocations();
    _texelWidthUniform = _program.getUniformLocation("texelWidth");
    _texelHeightUniform = _program.getUniformLocation("texelHeight");
    _upperThresholdUniform = _program.getUniformLocation("upperThreshold");
    _lowerThresholdUniform = _program.getUniformLocation("lowerThreshold");
}

void DirectionalNonmaximumSuppressionFilter::resolveUniformLocations()
{
    Filter::resolveUniformLocations();
    glUniform1f(_texelWidthUniform, _texelWidth);
    glUniform1f(_texelHeightUniform, _texelHeight);
    glUniform1f(_lowerThresholdUniform, _lowerThreshold);
    glUniform1f(_upperThresholdUniform, _upperThreshold);
}


