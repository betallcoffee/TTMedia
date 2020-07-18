//
//  TTBilateralFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 3/2/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTBilateralFilter.hpp"

using namespace TT;

static const GLchar* const kBilateralVetexShader = STRINGIZE
(
 attribute vec4 position;
 attribute vec4 texcoord;
 
 const int GAUSSIAN_SAMPLES = 9;
 
 uniform float texelWidthOffset;
 uniform float texelHeightOffset;
 
 varying vec2 v_texcoord;
 varying vec2 v_blurCoordinates[GAUSSIAN_SAMPLES];
 
 void main()
 {
     gl_Position = position;
     v_texcoord = texcoord.xy;
     
     // Calculate the positions for the blur
     int multiplier = 0;
     vec2 blurStep;
     vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);
     
     for (int i = 0; i < GAUSSIAN_SAMPLES; i++)
     {
         multiplier = i; //(i - ((GAUSSIAN_SAMPLES - 1) / 2));
         // Blur in x (horizontal)
         blurStep = float(multiplier) * singleStepOffset;
         v_blurCoordinates[i] = texcoord.xy + blurStep;
     }
 }
);

static const GLchar* const kBilateralFragmentShader = STRINGIZE
(
 uniform sampler2D texture;
 
 const lowp int GAUSSIAN_SAMPLES = 9;
 
 varying highp vec2 v_texcoord;
 varying highp vec2 v_blurCoordinates[GAUSSIAN_SAMPLES];
 
 uniform mediump float distanceNormalizationFactor;
 
 void main()
 {
     lowp vec4 centralColor;
     lowp float gaussianWeightTotal;
     lowp vec4 sum;
     lowp vec4 sampleColor;
     lowp float distanceFromCentralColor;
     lowp float gaussianWeight;
     
     centralColor = texture2D(texture, v_blurCoordinates[4]);
     gaussianWeightTotal = 0.18;
     sum = centralColor * 0.18;
     
     sampleColor = texture2D(texture, v_blurCoordinates[0]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[1]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[2]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[3]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[5]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[6]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[7]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;

     sampleColor = texture2D(texture, v_blurCoordinates[8]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     gl_FragColor = sum / gaussianWeightTotal;
 }
);

BilateralFilter::BilateralFilter() {
    setTexelSpacingMultiplier(4.0);
}

const GLchar* BilateralFilter::vertexShader() {
    return kBilateralVetexShader;
}

const GLchar* BilateralFilter::vertexShader2() {
    return kBilateralVetexShader;
}

const GLchar* BilateralFilter::fragmentShader() {
    return kBilateralFragmentShader;
}

const GLchar* BilateralFilter::fragmentShader2() {
    return kBilateralFragmentShader;
}

void BilateralFilter::getUniformLocations() {
    GaussianBlurFilter::getUniformLocations();
    _firstDistanceNormalizationFactorUniform = _program.getUniformLocation("distanceNormalizationFactor");
}

void BilateralFilter::resolveUniformLocations() {
    GaussianBlurFilter::resolveUniformLocations();
    
    glUniform1f(_firstDistanceNormalizationFactorUniform, _distanceNormalizationFactor);
}

void BilateralFilter::getUniformLocations2() {
    GaussianBlurFilter::getUniformLocations2();
    _secondDistanceNormalizationFactorUniform = _program2.getUniformLocation("distanceNormalizationFactor");
}

void BilateralFilter::resolveUniformLocations2() {
    GaussianBlurFilter::resolveUniformLocations2();
    
    glUniform1f(_secondDistanceNormalizationFactorUniform, _distanceNormalizationFactor);
}
