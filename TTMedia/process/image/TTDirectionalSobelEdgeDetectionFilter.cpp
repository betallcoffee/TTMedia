//
//  TTDirectionalSobelEdgeDetectionFilter.cpp
//  TTMedia
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTDirectionalSobelEdgeDetectionFilter.hpp"

using namespace TT;

static const GLchar* const kDirectionalSobelEdgeDetectionFragmentShader = STRINGIZE
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
     
     vec2 gradientDirection;
     gradientDirection.x = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
     gradientDirection.y = topLeftIntensity + 2.0 * topIntensity + topRightIntensity - bottomLeftIntensity - 2.0 * bottomIntensity - bottomRightIntensity;
     
     float gradientMagnitude = length(gradientDirection);
     vec2 normalizedDirection = normalize(gradientDirection);
     normalizedDirection = sign(normalizedDirection) * floor(abs(normalizedDirection) + 0.617316); // Offset by 1-sin(pi/8) to set to 0 if near axis, 1 if away
     normalizedDirection = (normalizedDirection + 1.0) * 0.5; // Place -1.0 - 1.0 within 0 - 1.0
     
     gl_FragColor = vec4(gradientMagnitude, normalizedDirection.x, normalizedDirection.y, 1.0);
 }
 );

DirectionalSobelEdgeDetectionFilter::DirectionalSobelEdgeDetectionFilter()
{
    
}

DirectionalSobelEdgeDetectionFilter::~DirectionalSobelEdgeDetectionFilter()
{
    
}

const GLchar *DirectionalSobelEdgeDetectionFilter::fragmentShader()
{
    return kDirectionalSobelEdgeDetectionFragmentShader;
}
