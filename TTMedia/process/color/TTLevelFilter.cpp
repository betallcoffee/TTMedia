//
//  TTLevelFilter.cpp
//  TTMedia
//
//  Created by liang on 12/5/18.
//  Copyright © 2018年 tina. All rights reserved.
//

//http://blog.sina.com.cn/s/blog_a5676788010192sx.html
//https://blog.csdn.net/candycat1992/article/details/21228945
//以float outRPixel  = GetPixelLevel(c.r);一句为例。
//
//GetPixelLevel函数接受c.r作为输入参数。由于tex2D得到的值范围在0.0到1.0之间，我们需要将其映射到0.0到255.0之间：
//[plain] view plain copy
//pixelResult = (pixelColor * 255.0);
//
//然后，我们减去_inBlack属性值，来使像素变暗。我们还要确保减去后的值不会小于0.0，因此使用max函数。
//[plain] view plain copy
//pixelResult = max(0, pixelResult - _inBlack);
//
//接下来，我们用(_inWhite - _inBlack)来得到新的white point值，并和pixelResult做除法。这相比与我们直接除以_inWhite，会调高像素的亮度，使它变得更亮。 然后使用_inGamma进行乘方操作。
//[plain] view plain copy
//pixelResult = saturate(pow(pixelResult / (_inWhite - _inBlack), _inGamma));
//
//最后，我们使用_outWhite和_outBlack来修改像素值，以便可以从全局上控制最小像素值（_outBlack）和最大像素值（_outWhite）。得到的结果需要除以255.0，来使得输出值的范围重新映射到0.0到1.0。
//[plain] view plain copy
//pixelResult = (pixelResult * (_outWhite - _outBlack) + _outBlack)/255.0;

#include "TTLevelFilter.hpp"

using namespace TT;

/*
 ** Gamma correction
 ** Details: http://blog.mouaif.org/2009/01/22/photoshop-gamma-correction-shader/
 */
// https://en.wikipedia.org/wiki/Gamma_correction

#define GammaCorrection(color, gamma)                                pow(color, 1.0 / gamma)

/*
 ** Levels control (input (+gamma), output)
 ** Details: http://blog.mouaif.org/2009/01/28/levels-control-shader/
 */

#define LevelsControlInputRange(color, minInput, maxInput)                min(max(color - minInput, vec3(0.0)) / (maxInput - minInput), vec3(1.0))
#define LevelsControlInput(color, minInput, gamma, maxInput)                GammaCorrection(LevelsControlInputRange(color, minInput, maxInput), gamma)
#define LevelsControlOutputRange(color, minOutput, maxOutput)             mix(minOutput, maxOutput, color)
#define LevelsControl(color, minInput, gamma, maxInput, minOutput, maxOutput)     LevelsControlOutputRange(LevelsControlInput(color, minInput, gamma, maxInput), minOutput, maxOutput)

static const GLchar *const kLevelFragmentShader = STRINGIZE
(
 varying highp vec2 v_texcoord;
 
 uniform sampler2D inputImageTexture;
 uniform mediump vec3 levelMinimum;
 uniform mediump vec3 levelMiddle;
 uniform mediump vec3 levelMaximum;
 uniform mediump vec3 minOutput;
 uniform mediump vec3 maxOutput;
 
 void main()
 {
     mediump vec4 textureColor = texture2D(inputImageTexture, v_texcoord);
     
     gl_FragColor = vec4(LevelsControl(textureColor.rgb, levelMinimum, levelMiddle, levelMaximum, minOutput, maxOutput), textureColor.a);
 }
);

LevelFilter::LevelFilter() {
    
}

LevelFilter::~LevelFilter() {
    
}

void LevelFilter::setRedLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut) {
    _minInVector.one = minIn;
    _gammaVector.one = gamma;
    _maxInVector.one = maxIn;
    _minOutputVector.one = minOut;
    _maxOutputVector.one = maxOut;
}

void LevelFilter::setRedLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn) {
    setRedLevels(minIn, gamma, maxIn, 0.0, 1.0);
}

void LevelFilter::setGreenLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut) {
    _minInVector.two = minIn;
    _gammaVector.two = gamma;
    _maxInVector.two = maxIn;
    _minOutputVector.two = minOut;
    _maxOutputVector.two = maxOut;
}

void LevelFilter::setGreenLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn) {
    setGreenLevels(minIn, gamma, maxIn, 0.0, 1.0);
}

void LevelFilter::setBlueLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut) {
    _minInVector.three = minIn;
    _gammaVector.three = gamma;
    _maxInVector.three = maxIn;
    _minOutputVector.three = minOut;
    _maxOutputVector.three = maxOut;
}

void LevelFilter::setBlueLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn) {
    setBlueLevels(minIn, gamma, maxIn, 0.0, 1.0);
}

void LevelFilter::setLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut) {
    setRedLevels(minIn, gamma, maxIn, minOut, maxOut);
    setGreenLevels(minIn, gamma, maxIn, minOut, maxOut);
    setBlueLevels(minIn, gamma, maxIn, minOut, maxOut);
}

void LevelFilter::setLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn) {
    setLevels(minIn, gamma, maxIn, 0.0, 1.0);
}

const GLchar *LevelFilter::fragmentShader() {
    return kLevelFragmentShader;
}

void LevelFilter::getUniformLocations() {
    Filter::getUniformLocations();
    
    _minInUniform = _program.getUniformLocation("levelMinimum");
    _gammaUniform = _program.getUniformLocation("levelMiddle");
    _maxInUniform = _program.getUniformLocation("levelMaximum");
    _minOutputUniform = _program.getUniformLocation("minOutput");
    _maxOutputUniform = _program.getUniformLocation("maxOutput");
}

void LevelFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniform3fv(_minInUniform, 1, (const GLfloat*)&_minInVector);
    glUniform3fv(_gammaUniform, 1, (const GLfloat*)&_gammaVector);
    glUniform3fv(_maxInUniform, 1, (const GLfloat*)&_maxInVector);
    glUniform3fv(_minOutputUniform, 1, (const GLfloat*)&_minOutputVector);
    glUniform3fv(_maxOutputUniform, 1, (const GLfloat*)&_maxOutputVector);
}


