//
//  TTTwoPassFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 26/1/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTTwoPassFilter_hpp
#define TTTwoPassFilter_hpp

#include "TTDef.h"
#include "TTFilter.hpp"
#include "TTFramebuffer.hpp"

namespace TT {
class TwoPassFilter : public Filter {
public:
    TwoPassFilter();
    ~TwoPassFilter();
    
    virtual void process(int64_t timestamp);
    
protected:
    
    virtual void createShader2() {}
    virtual void compileShader2();
    /**
     *返回自定义顶点 shader
     *其中变量约定：
     * attribute vec4 position;
     * attribute vec2 texcoord;
     * varying vec2 v_texcoord;
     */
    virtual const GLchar *vertexShader2();
    
    /**
     *返回自定义的片段 shader
     *其中约定 ：
     * varying highp vec2 v_texcoord; // 顶点 shader 传过来的 顶点信息
     * uniform sampler2D texture; // 输入纹理
     */
    virtual const GLchar *fragmentShader2();
    
    virtual void getAttribLocations2();
    virtual void resolveAttribLocations2();
    
    virtual void getUniformLocations2();
    virtual void resolveUniformLocations2();
    
    virtual const GLfloat *positionVertices2();
    virtual const GLfloat *texCoordForRotation2(TexRotations rotation);
    
    virtual void updateTexture2();
    
protected:
    TT_SP(Framebuffer) _firstFramebuffer = nullptr;
    Program _program2;
    
    GLuint _positionLocation2 = 0;
    GLuint _texCoordLocation2 = 0;
    GLuint _textureUniform2 = 0;
};
}

#endif /* TTTwoPassFilter_hpp */
