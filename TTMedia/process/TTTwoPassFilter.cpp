//
//  TTTwoPassFilter.cpp
//  TTMedia
//
//  Created by liang on 26/1/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTTwoPassFilter.hpp"
#include "TTGLContext_ios.h"
#include "TTFramebuffer.hpp"

using namespace TT;

TwoPassFilter::TwoPassFilter() {
    _firstFramebuffer = TT_MK_SP(Framebuffer)();
}

TwoPassFilter::~TwoPassFilter() {
    _firstFramebuffer.reset();
}

void TwoPassFilter::process(int64_t timestamp) {
    TIMED_FUNC(timer);
    GLContext::sharedProcessContext().use();
    
    PERFORMANCE_CHECKPOINT(timer);
    if (_firstFramebuffer->setUp(_width, _height)) {
        _firstFramebuffer->active();
        
        /// one pass
        if (!_program.isCompiled()) {
            createShader();
            compileShader();
        }
        _program.use();
        
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        updateTexture();
        
        resolveAttribLocations();
        resolveUniformLocations();
        
        PERFORMANCE_CHECKPOINT(timer);
        draw();
    }
    
    if (bindFramebuffer()) {
        /// two pass
        if (!_program2.isCompiled()) {
            createShader2();
            compileShader2();
        }
        
        _program2.use();
        
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        updateTexture2();
        
        resolveAttribLocations2();
        resolveUniformLocations2();
        
        PERFORMANCE_CHECKPOINT(timer);
        draw();
        
        PERFORMANCE_CHECKPOINT(timer);
        notifyFramebufferToFilters(timestamp);
        PERFORMANCE_CHECKPOINT(timer);
    }
}

void TwoPassFilter::compileShader2() {
    GLContext::sharedProcessContext().use();
    // TODO(liangliang) : process shader compile error.
    _program2.compile(vertexShader(), fragmentShader());
    getAttribLocations2();
    getUniformLocations2();
}

const GLchar *TwoPassFilter::vertexShader2() {
    return Filter::vertexShader();
}

const GLchar *TwoPassFilter::fragmentShader2() {
    return Filter::fragmentShader();
}

void TwoPassFilter::getAttribLocations2() {
    _positionLocation2 = _program2.getAttribLocation("position");
    _texCoordLocation2 = _program2.getAttribLocation("texcoord");
}

void TwoPassFilter::getUniformLocations2() {
    _textureUniform2 = _program2.getUniformLocation("texture");
}

void TwoPassFilter::resolveAttribLocations2() {
    const GLfloat *vertices = positionVertices2();
    glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, 0, 0, vertices);
    glEnableVertexAttribArray(_positionLocation2);
    
    // TODO config rotation
    const GLfloat *texCoord = texCoordForRotation2(kTexNoRotation);
    glVertexAttribPointer(_texCoordLocation2, 2, GL_FLOAT, 0, 0, texCoord);
    glEnableVertexAttribArray(_texCoordLocation2);
}

void TwoPassFilter::resolveUniformLocations2() {
    
}

const GLfloat *TwoPassFilter::positionVertices2() {
    return Program::positionVertices();
}

const GLfloat *TwoPassFilter::texCoordForRotation2(TexRotations rotation) {
    return Program::texCoordForRotation(rotation);
}

void TwoPassFilter::updateTexture2() {
    if (_firstFramebuffer) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _firstFramebuffer->textrue());
        
        /**
         *参数是 glActiveTexture 的顺序，0 开始计数
         */
        glUniform1i(_textureUniform, 3);
    }
}

