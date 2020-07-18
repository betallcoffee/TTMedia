//
//  TTBuffingFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 18/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTBuffingFilter.hpp"

using namespace TT;

static const GLchar* const kBuffingFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 varying highp vec2 v_texcoord2;
 varying highp vec2 v_texcoord3;
 uniform sampler2D texture;
 uniform sampler2D texture2;
 uniform sampler2D texture3;
 uniform mediump float smoothDegree;
 
 void main()
 {
    highp vec4 origin = texture2D(texture, v_texcoord);
    highp vec4 canny = texture2D(texture2, v_texcoord);
    highp vec4 bilateral = texture2D(texture3, v_texcoord3);
    highp vec4 smooth;
    lowp float r = origin.r;
    lowp float g = origin.g;
    lowp float b = origin.b;
    if (canny.r < 0.2 && r > 0.3725 && g > 0.1568 && b > 0.0784 && r > b && (max(max(r, g), b) - min(min(r, g), b)) > 0.0588 && abs(r-g) > 0.0588) {
        smooth = (1.0 - smoothDegree) * (origin - bilateral) + bilateral;
    }
    else {
        smooth = origin;
    }
    smooth.r = log(1.0 + 0.2 * smooth.r)/log(1.2);
    smooth.g = log(1.0 + 0.2 * smooth.g)/log(1.2);
    smooth.b = log(1.0 + 0.2 * smooth.b)/log(1.2);
    gl_FragColor = smooth;
}
 );

BuffingFilter::BuffingFilter()
{
    
}

BuffingFilter::~BuffingFilter()
{
    
}

const GLchar *BuffingFilter::fragmentShader()
{
    return kBuffingFragmentShader;
}

void BuffingFilter::getUniformLocations()
{
    ThreeInputFilter::getUniformLocations();
    
    _smoothDegreeUniform = _program.getUniformLocation("smoothDegree");
}

void BuffingFilter::resolveUniformLocations()
{
    ThreeInputFilter::resolveUniformLocations();
    
    glUniform1f(_smoothDegreeUniform, _smoothDegree);
}
