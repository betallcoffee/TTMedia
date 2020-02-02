//
//  TTGaussianBlurFilter.cpp
//  TTMedia
//
//  Created by liang on 25/1/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include <vector>
#include <complex>
#include "TTGaussianBlurFilter.hpp"

using namespace TT;

GaussianBlurFilter::GaussianBlurFilter() {
    createVertexShader();
    createFragmentShader();
}

void GaussianBlurFilter::setTexelSpacingMultiplier(float texelSpacing) {
    _texelSpacingMultiplier = texelSpacing;
    _verticalTexelSpacing = texelSpacing;
    _horizontalTexelSpacing = texelSpacing;
}

void GaussianBlurFilter::setBlurRadius(float radius) {
    _blurRadius = radius;
    createVertexShader();
    createFragmentShader();
}

const GLchar* GaussianBlurFilter::vertexShader() {
    return _vertexShader.c_str();
}

const GLchar* GaussianBlurFilter::vertexShader2() {
    return _vertexShader.c_str();
}

const GLchar* GaussianBlurFilter::fragmentShader() {
    return _fragmentShader.c_str();
}

void GaussianBlurFilter::getUniformLocations() {
    TwoPassFilter::getUniformLocations();
    _verticalPassTexelWidthOffsetUniform = _program.getUniformLocation("texelWidthOffset");
    _verticalPassTexelHeightOffsetUniform = _program.getUniformLocation("texelHeightOffset");
}

void GaussianBlurFilter::resolveUniformLocations() {
    TwoPassFilter::resolveUniformLocations();
    _verticalPassTexelHeightOffset = 0;
    _verticalPassTexelWidthOffset = 0;
    if (_height > 0) {
        _verticalPassTexelWidthOffset = _verticalTexelSpacing / _height;
    }
    
    glUniform1f(_verticalPassTexelWidthOffsetUniform, _verticalPassTexelWidthOffset);
    glUniform1f(_verticalPassTexelHeightOffsetUniform, _verticalPassTexelHeightOffset);
}

const GLchar* GaussianBlurFilter::fragmentShader2() {
    return _fragmentShader.c_str();
}

void GaussianBlurFilter::getUniformLocations2() {
    TwoPassFilter::getUniformLocations2();
    _horizontalPassTexelWidthOffsetUniform = _program.getUniformLocation("texelWidthOffset");
    _horizontalPassTexelHeightOffsetUniform = _program.getUniformLocation("texelHeightOffset");
}

void GaussianBlurFilter::resolveUniformLocations2() {
    TwoPassFilter::resolveUniformLocations2();
    _horizontalPassTexelHeightOffset = 0;
    _horizontalPassTexelWidthOffset = 0;
    if (_width > 0) {
        _horizontalPassTexelWidthOffset = _horizontalTexelSpacing / _width;
    }
    
    glUniform1f(_horizontalPassTexelWidthOffsetUniform, _horizontalPassTexelWidthOffset);
    glUniform1f(_horizontalPassTexelHeightOffsetUniform, _horizontalPassTexelHeightOffset);
}

void GaussianBlurFilter::createVertexShader() {
    _vertexShader.clear();
    _vertexShader.append("\
                         attribute vec4 position;\n\
                         attribute vec4 texcoord;\n\
                         \n\
                         uniform float texelWidthOffset;\n\
                         uniform float texelHeightOffset;\n\
                         \n\
                         varying vec2 v_blurCoordinates[");
    int32_t count = static_cast<int32_t>(_blurRadius * 2 + 1);
    _vertexShader.append(std::to_string(count));
    _vertexShader.append("];\n\
                         \n\
                         void main()\n\
                         {\n\
                         gl_Position = position;\n\
                         \n\
                         vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);\n");
    
    for (int32_t i = 0; i < count; i++) {
        int32_t offsetCenter = i - _blurRadius;
        if (offsetCenter < 0) {
            _vertexShader.append("v_blurCoordinates[");
            _vertexShader.append(std::to_string(i));
            _vertexShader.append("] = texcoord.xy - singleStepOffset * ");
            _vertexShader.append(std::to_string(-offsetCenter * 1.0));
            _vertexShader.append(";\n");
        } else if (offsetCenter > 0) {
            _vertexShader.append("v_blurCoordinates[");
            _vertexShader.append(std::to_string(i));
            _vertexShader.append("] = texcoord.xy + singleStepOffset * ");
            _vertexShader.append(std::to_string(offsetCenter * 1.0));
            _vertexShader.append(";\n");
        } else {
            _vertexShader.append("v_blurCoordinates[");
            _vertexShader.append(std::to_string(i));
            _vertexShader.append("] = texcoord.xy;\n");
        }
    }
    /// Inner offset loop
    _vertexShader.append("}\n");
}

void GaussianBlurFilter::createFragmentShader() {
    /// 1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) * exp(-pow(currentGaussianWeightIndex, 2.0) / (2.0 * pow(sigma, 2.0))
    /// First, generate the normal Gaussian weights for a given sigma
    float sumOfWeights = 0;
    std::vector<float> standardGaussianWeights;
    standardGaussianWeights.resize(_blurRadius + 1);
    for (int i = 0; i <= _blurRadius; i++) {
        standardGaussianWeights[i] = (1.0 / std::sqrt(2.0 * M_PI * std::pow(_sigma, 2.0))) * std::exp(-pow(i, 2.0) / (2.0 * std::pow(_sigma, 2.0)));
        
        if (0 == i) {
            sumOfWeights += standardGaussianWeights[i];
        } else {
            sumOfWeights += 2 * standardGaussianWeights[i];
        }
    }
    
    // Next, normalize these weights to prevent the clipping of the Gaussian curve at the end of the discrete samples from reducing luminance
    for (int i = 0; i <= _blurRadius; i++) {
        standardGaussianWeights[i] = standardGaussianWeights[i] / sumOfWeights;
    }
    
    _fragmentShader.clear();
    _fragmentShader.append("uniform sampler2D texture;\n\
                           \n\
                           varying highp vec2 v_blurCoordinates[");
    int32_t count = static_cast<int32_t>(_blurRadius * 2 + 1);
    _fragmentShader.append(std::to_string(count));
    _fragmentShader.append("];\n\
                           \n\
                           void main()\n\
                           {\n\
                           lowp vec4 sum = vec4(0.0);\n");
    
    /// Inner texture loop
    for (int32_t i = 0; i < count; i++) {
        int32_t offsetCenter = i - _blurRadius;
        if (offsetCenter < 0) {
            _fragmentShader.append("sum += texture2D(texture, v_blurCoordinates[");
            _fragmentShader.append(std::to_string(i));
            _fragmentShader.append("]) * ");
            _fragmentShader.append(std::to_string(standardGaussianWeights[-offsetCenter]));
            _fragmentShader.append(";\n");
        } else {
            _fragmentShader.append("sum += texture2D(texture, v_blurCoordinates[");
            _fragmentShader.append(std::to_string(i));
            _fragmentShader.append("]) * ");
            _fragmentShader.append(std::to_string(standardGaussianWeights[offsetCenter]));
            _fragmentShader.append(";\n");
        }
    }
    
    _fragmentShader.append("gl_FragColor = sum;\n\
                           }\n");
}


