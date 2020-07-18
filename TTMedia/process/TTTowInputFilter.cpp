//
//  TTTowInputFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTTowInputFilter.hpp"
#include "TTFramebuffer.hpp"

using namespace TT;

static const GLchar *const kTowInputVertexShader = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 texcoord;
 attribute vec2 texcoord2;
 varying vec2 v_texcoord;
 varying vec2 v_texcoord2;
 
 void main()
 {
     gl_Position = position;
     v_texcoord = texcoord.xy;
     v_texcoord2 = texcoord2.xy;
 }
 );

static const GLchar *const kTowInputFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 varying highp vec2 v_texcoord2;
 uniform sampler2D texture;
 uniform sampler2D texture2;
 
 void main()
 {
     gl_FragColor = texture2D(texture, v_texcoord);
 }
 );

TowInputFilter::TowInputFilter()
{
    
}

TowInputFilter::~TowInputFilter()
{
    
}

void TowInputFilter::setInputFramebuffer(TT_SP(Framebuffer) frameBuffer, int index)
{
    if (index == 1) {
        Filter::setInputFramebuffer(frameBuffer, index);
    } else if (index == 2) {
        _inputFramebuffer2 = frameBuffer;
    }
}

void TowInputFilter::notifyFramebufferToFilters(int64_t timestamp, int index)
{
    if (_hasReceivedTexture && _hasReceivedTexture2) {
        return;
    }
    
    if (index == 1) {
        _hasReceivedTexture = true;
    } else if (index == 2) {
        _hasReceivedTexture2 = true;
    }
    
    if (_hasReceivedTexture && _hasReceivedTexture2) {
        Filter::notifyFramebufferToFilters(timestamp);
        _hasReceivedTexture2 = false;
        _hasReceivedTexture = false;
    }
}

const GLchar *TowInputFilter::vertexShader()
{
    return kTowInputVertexShader;
}

const GLchar *TowInputFilter::fragmentShader()
{
    return kTowInputFragmentShader;
}

void TowInputFilter::getAttribLocations()
{
    Filter::getAttribLocations();
    _texCoordLocation2 = _program.getAttribLocation("texcoord2");
}

void TowInputFilter::getUniformLocations()
{
    Filter::getUniformLocations();
    _textureUniform2 = _program.getUniformLocation("texture2");
}

void TowInputFilter::resolveAttribLocations()
{
    Filter::resolveAttribLocations();
    
    // TODO config rotation
    const GLfloat *texCoord = texCoordForRotation(kTexNoRotation);
    glVertexAttribPointer(_texCoordLocation2, 2, GL_FLOAT, 0, 0, texCoord);
    glEnableVertexAttribArray(_texCoordLocation2);
}

void TowInputFilter::resolveUniformLocations()
{
    Filter::resolveUniformLocations();
}

void TowInputFilter::updateTexture()
{
    Filter::updateTexture();
    
    if (_inputFramebuffer2) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _inputFramebuffer2->textrue());
        
        /**
         *参数是 glActiveTexture 的顺序，0 开始计数
         */
        glUniform1i(_textureUniform2, 3);
    }
}
