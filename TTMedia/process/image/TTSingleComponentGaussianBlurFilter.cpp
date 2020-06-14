//
//  TTSingleComponentGaussianBlurFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 6/6/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include <complex>
#include <algorithm>

#include "TTSingleComponentGaussianBlurFilter.hpp"

using namespace TT;

SingleComponentGaussianBlurFilter::SingleComponentGaussianBlurFilter()
{
    
}

SingleComponentGaussianBlurFilter::~SingleComponentGaussianBlurFilter()
{
    
}

void SingleComponentGaussianBlurFilter::createGaussianWeights()
{
    if (_blurRadius < 1) {
        _standardGaussianWeights.clear();
        _optimizedGaussianWeights.clear();
        _optimizedGaussianOffsets.clear();
        return;
    }
    /// 1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) * exp(-pow(currentGaussianWeightIndex, 2.0) / (2.0 * pow(sigma, 2.0))
    /// First, generate the normal Gaussian weights for a given sigma
    float sumOfWeights = 0;
    _standardGaussianWeights.clear();
    _standardGaussianWeights.resize(_blurRadius + 1);
    for (int i = 0; i <= _blurRadius; i++) {
        _standardGaussianWeights[i] = (1.0 / std::sqrt(2.0 * M_PI * std::pow(_sigma, 2.0))) * std::exp(-pow(i, 2.0) / (2.0 * std::pow(_sigma, 2.0)));
        
        if (0 == i) {
            sumOfWeights += _standardGaussianWeights[i];
        } else {
            sumOfWeights += 2 * _standardGaussianWeights[i];
        }
    }
    
    // Next, normalize these weights to prevent the clipping of the Gaussian curve at the end of the discrete samples from reducing luminance
    for (int i = 0; i <= _blurRadius; i++) {
        _standardGaussianWeights[i] = _standardGaussianWeights[i] / sumOfWeights;
    }
    
    // From these weights we calculate the offsets to read interpolated values from
    int32_t temp = _blurRadius / 2 + ((int32_t)_blurRadius % 2);
    _numberOfOptimizedOffsets = std::min(temp, 7);
    _optimizedGaussianWeights.clear();
    _optimizedGaussianWeights.resize(_numberOfOptimizedOffsets);
    _optimizedGaussianOffsets.clear();
    _optimizedGaussianOffsets.resize(_numberOfOptimizedOffsets);
    for (int i = 0; i < _numberOfOptimizedOffsets; i++) {
        GLfloat firstWeight = _standardGaussianWeights[i * 2 + 1];
        GLfloat secondWeight = _standardGaussianWeights[i * 2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
        _optimizedGaussianWeights[i] = optimizedWeight;
        _optimizedGaussianOffsets[i] = (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) / optimizedWeight;
    }
}

void SingleComponentGaussianBlurFilter::createVertexShader()
{
    _vertexShader.clear();
    if (_blurRadius < 1) {
        return;
    }
    
    createGaussianWeights();
    _vertexShader.append("\
                         attribute vec4 position;\n\
                         attribute vec4 texcoord;\n\
                         \n\
                         uniform float texelWidthOffset;\n\
                         uniform float texelHeightOffset;\n\
                         \n\
                         varying vec2 v_blurCoordinates[");
    int32_t count = static_cast<int32_t>(_numberOfOptimizedOffsets * 2 + 1);
    _vertexShader.append(std::to_string(count));
    _vertexShader.append("];\n\
                         \n\
                         void main()\n\
                         {\n\
                         gl_Position = position;\n\
                         \n\
                         vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);\n");
    _vertexShader.append("v_blurCoordinates[0] = texcoord.xy;\n");
    for (int32_t i = 0; i < count; i++) {
        _vertexShader.append("v_blurCoordinates[");
        _vertexShader.append(std::to_string(i * 2 + 1));
        _vertexShader.append("] = texcoord.xy + singleStepOffset * ");
        _vertexShader.append(std::to_string(_optimizedGaussianOffsets[i]));
        _vertexShader.append(";\n");
        _vertexShader.append("v_blurCoordinates[");
        _vertexShader.append(std::to_string(i * 2 + 2));
        _vertexShader.append("] = texcoord.xy - singleStepOffset * ");
        _vertexShader.append(std::to_string(_optimizedGaussianOffsets[i]));
        _vertexShader.append(";\n");
        
    }
    /// Inner offset loop
    _vertexShader.append("}\n");
}

void SingleComponentGaussianBlurFilter::createFragmentShader()
{
    _fragmentShader.clear();
    if (_blurRadius < 1) {
        return;
    }
    
    _fragmentShader.append("uniform sampler2D texture;\n\
                           \n\
                           varying highp vec2 v_blurCoordinates[");
    int32_t count = static_cast<int32_t>(_blurRadius * 2 + 1);
    _fragmentShader.append(std::to_string(count));
    _fragmentShader.append("];\n\
                           \n\
                           void main()\n\
                           {\n\
                           float sum = 0.0;\n");
    
    /// Inner texture loop
    for (int32_t i = 0; i < count; i++) {
        GLfloat optimizedWeight = _optimizedGaussianWeights[i];
        _fragmentShader.append("sum += texture2D(texture, v_blurCoordinates[");
        _fragmentShader.append(std::to_string(i * 2 + 1));
        _fragmentShader.append("]).r * ");
        _fragmentShader.append(std::to_string(optimizedWeight));
        _fragmentShader.append(";\n");
        _fragmentShader.append("sum += texture2D(texture, v_blurCoordinates[");
        _fragmentShader.append(std::to_string(i * 2 + 2));
        _fragmentShader.append("]).r * ");
        _fragmentShader.append(std::to_string(optimizedWeight));
        _fragmentShader.append(";\n");
    }
    
    _fragmentShader.append("gl_FragColor = vec4(sum, sum, sum, 1.0);\n\
                           }\n");
}



