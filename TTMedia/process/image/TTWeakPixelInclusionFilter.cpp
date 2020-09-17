//
//  TTWeakPixelInclusionFilter.cpp
//  TTMedia
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTWeakPixelInclusionFilter.hpp"

using namespace TT;

static const GLchar* const kWeakPixelInclusionFragmentShader = STRINGIZE
(
 precision mediump float;
 
 uniform sampler2D texture;
 
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
     float bottomLeftIntensity = texture2D(texture, v_bottomLeftTextureCoordinate).r;
     float topRightIntensity = texture2D(texture, v_topRightTextureCoordinate).r;
     float topLeftIntensity = texture2D(texture, v_topLeftTextureCoordinate).r;
     float bottomRightIntensity = texture2D(texture, v_bottomRightTextureCoordinate).r;
     float leftIntensity = texture2D(texture, v_leftTextureCoordinate).r;
     float rightIntensity = texture2D(texture, v_rightTextureCoordinate).r;
     float bottomIntensity = texture2D(texture, v_bottomTextureCoordinate).r;
     float topIntensity = texture2D(texture, v_topTextureCoordinate).r;
     
     float pixelIntensitySum = bottomLeftIntensity + topRightIntensity + topLeftIntensity + bottomRightIntensity + leftIntensity + rightIntensity + bottomIntensity + topIntensity + centerIntensity;
     float sumTest = step(1.5, pixelIntensitySum);
     float pixelTest = step(0.01, centerIntensity);
          
     gl_FragColor = vec4(vec3(sumTest * pixelTest), 1.0);
 }
 );

WeakPixelInclusionFilter::WeakPixelInclusionFilter()
{
    
}

WeakPixelInclusionFilter::~WeakPixelInclusionFilter()
{
    
}

const GLchar *WeakPixelInclusionFilter::fragmentShader()
{
    return kWeakPixelInclusionFragmentShader;
}
