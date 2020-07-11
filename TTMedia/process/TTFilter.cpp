//
//  TTFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 15/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTFilter.hpp"

#include "TTFramebuffer.hpp"
#include "TTGLContext_ios.h"

using namespace TT;

static const GLchar *const kVertexShader = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 texcoord;
 varying vec2 v_texcoord;
 
 void main()
 {
     gl_Position = position;
     v_texcoord = texcoord.xy;
 }
 );

static const GLchar *const kFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 void main()
 {
     gl_FragColor = texture2D(texture, v_texcoord);
 }
 );

Filter::Filter() :
 _srcFramebuffer(nullptr), _framebuffer(nullptr),
 _width(0), _height(0),
 _positionLocation(0), _texCoordLocation(0), _textureUniform(0) {
    _framebuffer = std::make_shared<Framebuffer>();
}

Filter::~Filter() {
    _srcFramebuffer.reset();
    _framebuffer.reset();
    removeAllOutputs();
}

void Filter::addOutput(std::shared_ptr<Filter> output, int index) {
    if (output && index >= 0) {
        _outputs[index] = output;
    }
}

void Filter::removeOutput(std::shared_ptr<Filter> output, int index) {
    if (output && index >= 0) {
        _outputs.erase(index);
    }
}

void Filter::removeAllOutputs() {
    std::map<int, std::shared_ptr<Filter>>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); it++) {
        it->second.reset();
    }
    _outputs.clear();
}


void Filter::notifyFramebufferToFilters(int64_t timestamp) {
    std::map<int, std::shared_ptr<Filter>>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); it++) {
        it->second->setSrcFramebuffer(_framebuffer);
        it->second->process(timestamp);
    }
}

void Filter::setSrcFramebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    _srcFramebuffer = framebuffer;
    if (_srcFramebuffer) {
        _width = _srcFramebuffer->width();
        _height = _srcFramebuffer->height();
        setupFilterSize();
    } else {
        _width = 0;
        _height = 0;
        setupFilterSize();
    }
}

void Filter::process(int64_t timestamp) {
    TIMED_FUNC(timer);
    GLContext::sharedProcessContext().use();
    
    PERFORMANCE_CHECKPOINT(timer);
    if (bindFramebuffer()) {
        if (!_program.isCompiled())
            compileShader();
        _program.use();
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        updateTexture();
        
        resolveAttribLocations();
        resolveUniformLocations();
        
        PERFORMANCE_CHECKPOINT(timer);
        draw();
        
        PERFORMANCE_CHECKPOINT(timer);
        notifyFramebufferToFilters(timestamp);
        PERFORMANCE_CHECKPOINT(timer);
    }
}

void Filter::setupFilterSize() {
    
}

bool Filter::bindFramebuffer() {
    if (_framebuffer->setUp(_width, _height)) {
        _framebuffer->active();
        return true;
    }
    return false;
}

void Filter::compileShader() {
    GLContext::sharedProcessContext().use();
    // TODO(liangliang) : process shader compile error.
    _program.compile(vertexShader(), fragmentShader());
    getAttribLocations();
    getUniformLocations();
}

const GLchar *Filter::vertexShader() {
    return kVertexShader;
}

const GLchar *Filter::fragmentShader() {
    return kFragmentShader;
}

void Filter::getAttribLocations() {
    _positionLocation = _program.getAttribLocation("position");
    _texCoordLocation = _program.getAttribLocation("texcoord");
}

void Filter::getUniformLocations() {
    _textureUniform = _program.getUniformLocation("texture");
}

void Filter::resolveAttribLocations() {
    const GLfloat *vertices = positionVertices();
    glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, 0, 0, vertices);
    glEnableVertexAttribArray(_positionLocation);
    
    // TODO config rotation
    const GLfloat *texCoord = texCoordForRotation(kTexNoRotation);
    glVertexAttribPointer(_texCoordLocation, 2, GL_FLOAT, 0, 0, texCoord);
    glEnableVertexAttribArray(_texCoordLocation);
}

void Filter::resolveUniformLocations() {
    
}

const GLfloat *Filter::positionVertices() {
    return Program::positionVertices();
}

const GLfloat *Filter::texCoordForRotation(TexRotations rotation) {
    return Program::texCoordForRotation(rotation);
}

void Filter::updateTexture() {
    if (_srcFramebuffer) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _srcFramebuffer->textrue());
        
        /**
         *参数是 glActiveTexture 的顺序，0 开始计数
         */
        glUniform1i(_textureUniform, 2);
    }
}

void Filter::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
