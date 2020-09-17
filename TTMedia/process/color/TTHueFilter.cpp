//
//  TTHueFilter.cpp
//  TTMedia
//
//  Created by liang on 12/5/18.
//  Copyright © 2018年 tina. All rights reserved.
//  Adapted from http://stackoverflow.com/questions/9234724/how-to-change-hue-of-a-texture-with-glsl - see for code and discussion

//Once in one of those spaces, you can get the hue from the angle formed by the chroma channels, by doing hue = atan(cr/cb) (watching for cb == 0). This gives you a value in radians. Simply rotate it by adding the hue rotation amount. Once you've done that, you can calculate the magnitude of the chroma with chroma = sqrt(cr*cr+cb*cb). To get back to RGB, calculate the new Cb and Cr (or I & Q) using Cr = chroma * sin (hue), Cb = chroma * cos (hue). Then convert back to RGB as described on the above web pages.

#include "TTHueFilter.hpp"

using namespace TT;

static const GLchar *const kHueFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float hueAdjust;
 
 const highp  vec4  kRGBToYPrime = vec4 (0.299, 0.587, 0.114, 0.0);
 const highp  vec4  kRGBToI     = vec4 (0.595716, -0.274453, -0.321263, 0.0);
 const highp  vec4  kRGBToQ     = vec4 (0.211456, -0.522591, 0.31135, 0.0);
 
 const highp  vec4  kYIQToR   = vec4 (1.0, 0.9563, 0.6210, 0.0);
 const highp  vec4  kYIQToG   = vec4 (1.0, -0.2721, -0.6474, 0.0);
 const highp  vec4  kYIQToB   = vec4 (1.0, -1.1070, 1.7046, 0.0);
 
 void main ()
 {
     // Sample the input pixel
     highp vec4 color   = texture2D(inputImageTexture, v_texcoord);
     
     // Convert to YIQ
     highp float   YPrime  = dot (color, kRGBToYPrime);
     highp float   I      = dot (color, kRGBToI);
     highp float   Q      = dot (color, kRGBToQ);
     
     // Calculate the hue and chroma
     highp float   hue     = atan (Q, I);
     highp float   chroma  = sqrt (I * I + Q * Q);
     
     // Make the user's adjustments
     hue += (-hueAdjust); //why negative rotation?
     
     // Convert back to YIQ
     Q = chroma * sin (hue);
     I = chroma * cos (hue);
     
     // Convert back to RGB
     highp vec4    yIQ   = vec4 (YPrime, I, Q, 0.0);
     color.r = dot (yIQ, kYIQToR);
     color.g = dot (yIQ, kYIQToG);
     color.b = dot (yIQ, kYIQToB);
     
     // Save the result
     gl_FragColor = color;
 }
 );

HueFilter::HueFilter() {
    
}

HueFilter::~HueFilter() {
    
}

void HueFilter::setHueAdjust(float hueAdjust) {
    _hueAdjust = hueAdjust;
}

const GLchar *HueFilter::fragmentShader() {
    return kHueFragmentShader;
}

void HueFilter::getUniformLocations() {
    Filter::getUniformLocations();
    _hueAdjustUniform = _program.getUniformLocation("hueAdjust");
}

void HueFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniform1f(_hueAdjustUniform, _hueAdjust);
}
