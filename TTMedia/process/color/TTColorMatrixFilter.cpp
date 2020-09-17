//
//  TTColorMatrixFilter.cpp
//  TTMedia
//
//  Created by liang on 21/1/20.
//  Copyright © 2020年 tina. All rights reserved.
//

#include "TTColorMatrixFilter.hpp"

using namespace TT;

static const GLchar *const kColorMatrixFragmentShader = STRINGIZE
(
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 uniform lowp mat4 colorMatrix;
 uniform lowp float intensity;
 
 void main()
 {
    lowp vec4 textureColor = texture2D(texture, v_texcoord);
    lowp vec4 outputColor = textureColor * colorMatrix;
    gl_FragColor = outputColor;
    
}
 );
//gl_FragColor = (intensity * outputColor) + ((1.0 - intensity) * textureColor);
ColorMatrixFilter::ColorMatrixFilter() {
    _colorMatrix = (GPUMatrix4x4){
        {1.f, 0.f, 0.f, 0.f},
        {0.f, 1.f, 0.f, 0.f},
        {0.f, 0.f, 1.f, 0.f},
        {0.f, 0.f, 0.f, 1.f}
    };
}

void ColorMatrixFilter::setColorMatrix(TT::GPUMatrix4x4 &colorMatrix) {
    _colorMatrix.one.one = colorMatrix.one.one;
    _colorMatrix.one.two = colorMatrix.one.two;
    _colorMatrix.one.three = colorMatrix.one.three;
    _colorMatrix.one.four = colorMatrix.one.four;
    
    _colorMatrix.two.one = colorMatrix.two.one;
    _colorMatrix.two.two = colorMatrix.two.two;
    _colorMatrix.two.three = colorMatrix.two.three;
    _colorMatrix.two.four = colorMatrix.two.four;
    
    _colorMatrix.three.one = colorMatrix.three.one;
    _colorMatrix.three.two = colorMatrix.three.two;
    _colorMatrix.three.three = colorMatrix.three.three;
    _colorMatrix.three.four = colorMatrix.three.four;
    
    _colorMatrix.four.one = colorMatrix.four.one;
    _colorMatrix.four.two = colorMatrix.four.two;
    _colorMatrix.four.three = colorMatrix.four.three;
    _colorMatrix.four.four = colorMatrix.four.four;
}

void ColorMatrixFilter::setIntensity(float intensity) {
    _intensity = intensity;
}

const GLchar* ColorMatrixFilter::fragmentShader() {
    return kColorMatrixFragmentShader;
}

void ColorMatrixFilter::getUniformLocations() {
    Filter::getUniformLocations();
    _colorMatrixUniform = _program.getUniformLocation("colorMatrix");
    _intensityUniform = _program.getUniformLocation("intensity");
}

void ColorMatrixFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniformMatrix4fv(_colorMatrixUniform, 1, GL_FALSE, (GLfloat *)&_colorMatrix);
//    glUniform1f(_intensityUniform, _intensity);
}


