//
//  TTThreeInputFilter.cpp
//  TTMedia
//
//  Created by liang on 18/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTThreeInputFilter.hpp"
#include "TTFramebuffer.hpp"

using namespace TT;

static const GLchar *const kThreeInputVertexShader = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 texcoord;
 attribute vec2 texcoord2;
 attribute vec2 texcoord3;
 varying vec2 v_texcoord;
 varying vec2 v_texcoord2;
 varying vec2 v_texcoord3;
 
 void main()
 {
     gl_Position = position;
     v_texcoord = texcoord.xy;
     v_texcoord2 = texcoord2.xy;
     v_texcoord3 = texcoord3.xy;
 }
 );

static const GLchar *const kThreeInputFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 varying highp vec2 v_texcoord2;
 varying highp vec2 v_texcoord3;
 uniform sampler2D texture;
 uniform sampler2D texture2;
 uniform sampler2D texture3;
 
 void main()
 {
     gl_FragColor = texture2D(texture, v_texcoord);
 }
 );

ThreeInputFilter::ThreeInputFilter()
{
    
}

ThreeInputFilter::~ThreeInputFilter()
{
    
}

void ThreeInputFilter::setInputFramebuffer(TT_SP(Framebuffer) frameBuffer, int index)
{
    if (index == 1 || index == 2) {
        TowInputFilter::setInputFramebuffer(frameBuffer, index);
    } else if (index == 3) {
        _inputFramebuffer3 = frameBuffer;
    }
}

void ThreeInputFilter::notifyFramebufferToFilters(int64_t timestamp, int index)
{
    if (_hasReceivedTexture && _hasReceivedTexture2 && _hasReceivedTexture3) {
        return;
    }
    
    if (index == 1) {
        _hasReceivedTexture = true;
    } else if (index == 2) {
        _hasReceivedTexture2 = true;
    } else if (index == 3) {
        _hasReceivedTexture3 = true;
    }
    
    if (_hasReceivedTexture && _hasReceivedTexture2 && _hasReceivedTexture3) {
        Filter::notifyFramebufferToFilters(timestamp);
        _hasReceivedTexture = false;
        _hasReceivedTexture2 = false;
        _hasReceivedTexture3 = false;
    }
}

const GLchar *ThreeInputFilter::vertexShader()
{
    return kThreeInputVertexShader;
}

const GLchar *ThreeInputFilter::fragmentShader()
{
    return kThreeInputFragmentShader;
}

void ThreeInputFilter::getAttribLocations()
{
    TowInputFilter::getAttribLocations();
    _texCoordLocation3 = _program.getAttribLocation("texcoord3");
}

void ThreeInputFilter::getUniformLocations()
{
    TowInputFilter::getUniformLocations();
    _textureUniform3 = _program.getUniformLocation("texture3");
}

void ThreeInputFilter::resolveAttribLocations()
{
    TowInputFilter::resolveAttribLocations();
    
    // TODO config rotation
    const GLfloat *texCoord = texCoordForRotation(kTexNoRotation);
    glVertexAttribPointer(_texCoordLocation3, 2, GL_FLOAT, 0, 0, texCoord);
    glEnableVertexAttribArray(_texCoordLocation3);
}

void ThreeInputFilter::resolveUniformLocations()
{
    TowInputFilter::resolveUniformLocations();
}

void ThreeInputFilter::updateTexture()
{
    TowInputFilter::updateTexture();
    
    if (_inputFramebuffer3) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, _inputFramebuffer3->textrue());
        
        /**
         *参数是 glActiveTexture 的顺序，0 开始计数
         */
        glUniform1i(_textureUniform3, 4);
    }
}
