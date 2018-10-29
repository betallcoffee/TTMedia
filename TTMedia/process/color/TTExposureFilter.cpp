//
//  TTExposureFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 10/5/18.
//  Copyright © 2018年 tina. All rights reserved.
//  曝光

#include "TTExposureFilter.hpp"

using namespace TT;

static const GLchar *const kExposureFragmentShader = STRINGIZE
(
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 uniform highp float exposure;
 
 void main()
 {
     highp vec4 textureColor = texture2D(texture, v_texcoord);
     
     gl_FragColor = vec4(textureColor.rgb * pow(2.0, exposure), textureColor.w);
 }
 );

ExposureFilter::ExposureFilter() : _exposure(0.0), _exposureUniform(kGLInvalid) {
    
}

ExposureFilter::~ExposureFilter() {
    
}

void ExposureFilter::setExposure(float exposure) {
    _exposure = exposure;
}

const GLchar *ExposureFilter::fragmentShader() {
    return kExposureFragmentShader;
}

void ExposureFilter::getUniformLocations() {
    Filter::getUniformLocations();
    _exposureUniform = _program.getUniformLocation("exposure");
}

void ExposureFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniform1f(_exposureUniform, _exposure);
}
