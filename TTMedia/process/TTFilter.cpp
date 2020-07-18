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
 _inputFramebuffer(nullptr), _outputFramebuffer(nullptr),
 _width(0), _height(0),
 _positionLocation(0), _texCoordLocation(0), _textureUniform(0) {
    _outputFramebuffer = std::make_shared<Framebuffer>();
}

Filter::~Filter() {
    _inputFramebuffer.reset();
    _outputFramebuffer.reset();
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

void Filter::setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer, int index) {
    _inputFramebuffer = framebuffer;
    if (_inputFramebuffer) {
        _width = _inputFramebuffer->width();
        _height = _inputFramebuffer->height();
        setupFilterSize();
    } else {
        _width = 0;
        _height = 0;
        setupFilterSize();
    }
}

void Filter::notifyFramebufferToFilters(int64_t timestamp, int index) {
    std::map<int, std::shared_ptr<Filter>>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); it++) {
        it->second->setInputFramebuffer(_outputFramebuffer, it->first);
        it->second->process(timestamp, it->first);
    }
}

void Filter::process(int64_t timestamp, int index) {
    TIMED_FUNC(timer);
    // 1. 设置 opengl context
    GLContext::sharedProcessContext().use();
    
    PERFORMANCE_CHECKPOINT(timer);
    // 2. 设置 frame buffer, 包含输出 width/height 和绘制到纹理
    if (bindFramebuffer()) {
        // 3. 编译和设置 shader
        if (!_program.isCompiled()) {
            createShader();
            compileShader();
        }
        
        _program.use();
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 4. 上传纹理数据到 gpu
        updateTexture();
        
        // 5. 设置 shader 中的 atrrib 变量值
        resolveAttribLocations();
        // 6. 设置 shader 中的 uniform 变量值
        resolveUniformLocations();
        
        PERFORMANCE_CHECKPOINT(timer);
        // 7. 绘制，执行 opengl pipeline
        draw();
        
        PERFORMANCE_CHECKPOINT(timer);
        // 8. 将绘制结果纹理传递后续 filter
        notifyFramebufferToFilters(timestamp, index);
        PERFORMANCE_CHECKPOINT(timer);
    }
}

void Filter::setupFilterSize() {
    
}

bool Filter::bindFramebuffer() {
    if (_outputFramebuffer->setUp(_width, _height)) {
        _outputFramebuffer->active();
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
    if (_inputFramebuffer) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _inputFramebuffer->textrue());
        
        /**
         *参数是 glActiveTexture 的顺序，0 开始计数
         */
        glUniform1i(_textureUniform, 2);
    }
}

void Filter::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
