//
//  TTGrayscaleFilter.cpp
//  TTMedia
//
//  Created by liang on 6/6/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTGrayscaleFilter.hpp"

using namespace TT;

static const GLchar * const kGrayscaleFragmentShader = STRINGIZE
(
 precision highp float;
 
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     lowp vec4 textureColor = texture2D(texture, v_texcoord);
     float luminance = dot(textureColor.rgb, W);
     
     gl_FragColor = vec4(vec3(luminance), textureColor.a);
 }
 );

GrayscaleFilter::GrayscaleFilter() {
    
}

GrayscaleFilter::~GrayscaleFilter() {
    
}

const GLchar *GrayscaleFilter::fragmentShader() {
    return kGrayscaleFragmentShader;
}

