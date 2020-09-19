//
//  TTConvolution3x3Filter.cpp
//  TTMedia
//
//  Created by liang on 19/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTConvolution3x3Filter.hpp"

using namespace TT;

static const GLchar* const kConvolution3x3FragmentShader = STRINGIZE
(
 precision highp float;

 uniform sampler2D texture;
 uniform mediump mat3 convolutionKernel;
 
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
     mediump vec3 bottomColor = texture2D(texture, v_bottomTextureCoordinate).rgb;
     mediump vec3 bottomLeftColor = texture2D(texture, v_bottomLeftTextureCoordinate).rgb;
     mediump vec3 bottomRightColor = texture2D(texture, v_bottomRightTextureCoordinate).rgb;
     mediump vec4 centerColor = texture2D(texture, v_textureCoordinate);
     mediump vec3 leftColor = texture2D(texture, v_leftTextureCoordinate).rgb;
     mediump vec3 rightColor = texture2D(texture, v_rightTextureCoordinate).rgb;
     mediump vec3 topColor = texture2D(texture, v_topTextureCoordinate).rgb;
     mediump vec3 topRightColor = texture2D(texture, v_topRightTextureCoordinate).rgb;
     mediump vec3 topLeftColor = texture2D(texture, v_topLeftTextureCoordinate).rgb;

     mediump vec3 resultColor = topLeftColor * convolutionKernel[0][0] + topColor * convolutionKernel[0][1] + topRightColor * convolutionKernel[0][2];
     resultColor += leftColor * convolutionKernel[1][0] + centerColor.rgb * convolutionKernel[1][1] + rightColor * convolutionKernel[1][2];
     resultColor += bottomLeftColor * convolutionKernel[2][0] + bottomColor * convolutionKernel[2][1] + bottomRightColor * convolutionKernel[2][2];

     gl_FragColor = vec4(resultColor, centerColor.a);
 }
 );

Convolution3x3Filter::Convolution3x3Filter()
{
    _convolutionKernel = {
        {0.f, 0.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 0.f, 0.f}
    };
}

Convolution3x3Filter::~Convolution3x3Filter()
{
    
}

const GLchar *Convolution3x3Filter::fragmentShader()
{
    return kConvolution3x3FragmentShader;
}

void Convolution3x3Filter::getUniformLocations()
{
    Texture3x3SamplingFilter::getUniformLocations();
    
    _convolutionKernelUniform = _program.getUniformLocation("convolutionKernel");
}

void Convolution3x3Filter::resolveUniformLocations()
{
    Texture3x3SamplingFilter::resolveUniformLocations();
    
    glUniformMatrix3fv(_convolutionKernelUniform, 1, GL_FALSE, (GLfloat *)&_convolutionKernelUniform);
}

