//
//  TTThreeInputFilter.hpp
//  TTMedia
//
//  Created by liang on 18/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTThreeInputFilter_hpp
#define TTThreeInputFilter_hpp

#include "TTTowInputFilter.hpp"

namespace TT {

/**
支持三个纹理输入的处理类，绑定已经上传的 rgb 纹理，
纹理1 绑定 GL_TEXTURE2 ，纹理 2 绑定 GL_TEXTURE3， 纹理 2 绑定 GL_TEXTURE4
进行处理，并输出到纹理 GL_TEXTURE1
*/
class ThreeInputFilter : public TowInputFilter {
public:
    ThreeInputFilter();
    ~ThreeInputFilter();
    
    virtual void setInputFramebuffer(TT_SP(Framebuffer) frameBuffer, int index = 0);
    virtual void notifyFramebufferToFilters(int64_t timestamp, int index = 0);
    
    /**
     *返回自定义顶点 shader
     *其中变量约定：
     * attribute vec4 position; // 顶点坐标
     * attribute vec2 texcoord; // 纹理1坐标
     * attribute vec2 texcoord2; // 纹理2坐标
     * attribute vec2 texcoord3; // 纹理3坐标
     * varying vec2 v_texcoord; // 转换后的纹理1坐标
     * varying vec2 v_texcoord2; // 转换后的纹理2坐标
     * varying vec2 v_texcoord2; // 转换后的纹理3坐标
     */
    virtual const GLchar *vertexShader();
    
    /**
     *返回自定义的片段 shader
     *其中约定 ：
     * varying highp vec2 v_texcoord; // 顶点 shader 传过来的纹理1坐标信息
     * varying highp vec2 v_texcoord2; // 顶点 shader 传过来的纹理2坐标信息
     * varying highp vec2 v_texcoord3; // 顶点 shader 传过来的纹理3坐标信息
     * uniform sampler2D texture; // 输入纹理1
     * uniform sampler2D texture2; // 输入纹理2
     * uniform sampler2D texture3; // 输入纹理3
     */
    virtual const GLchar *fragmentShader();
    
    /**
     * shader 中的 attrib 和 uniform 变量在每次 process 都会赋值
     */
    virtual void getAttribLocations();
    virtual void resolveAttribLocations();
    
    virtual void getUniformLocations();
    virtual void resolveUniformLocations();
    
    virtual void updateTexture();
    
private:
    TT_SP(Framebuffer) _inputFramebuffer3 = nullptr;
    GLuint _texCoordLocation3 = 0;
    GLuint _textureUniform3 = 0;
    
    bool _hasReceivedTexture = false;
    bool _hasReceivedTexture2 = false;
    bool _hasReceivedTexture3 = false;
};

}

#endif /* TTThreeInputFilter_hpp */
