//
//  TTFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 15/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//
// 基础的纹理处理类，绑定已经上传的 rgb 纹理到 GL_TEXTURE2 ，进行处理，并输出到纹理 GL_TEXTURE1

#ifndef TTFilter_hpp
#define TTFilter_hpp

#include <memory>
#include <map>

#include <OpenGLES/ES2/gl.h>

#include "TTdef.h"
#include "TTOpenGL.h"
#include "TTClass.hpp"
#include "TTProgram.hpp"
#include "TTFilterCommon.h"

namespace TT {
    class Framebuffer;
    
    class Filter {
    public:
        Filter();
        virtual ~Filter();
        
        void addOutput(std::shared_ptr<Filter> output, int index = 0);
        void removeOutput(std::shared_ptr<Filter> output, int index = 0);
        void removeAllOutputs();

        /**
         *输入数据
         */
        void setSrcFramebuffer(std::shared_ptr<Framebuffer> frameBuffer);
        std::shared_ptr<const Framebuffer> srcFramebuffer() { return _srcFramebuffer; }
        
        virtual void process(int64_t timestamp);
        
        virtual void notifyFramebufferToFilters(int64_t timestamp);
        
    protected:
        virtual void setupFilterSize();
        
        virtual bool bindFramebuffer();
        
        /**
         * 动态创建生成的 shader，将 program reset 则会重新生成 shader
         */
        virtual void createShader() {}
        virtual void compileShader();
        /**
         *返回自定义顶点 shader
         *其中变量约定：
         * attribute vec4 position; // 顶点坐标
         * attribute vec2 texcoord; // 纹理坐标
         * varying vec2 v_texcoord; // 转换后的纹理坐标
         */
        virtual const GLchar *vertexShader();
        
        /**
         *返回自定义的片段 shader
         *其中约定 ：
         * varying highp vec2 v_texcoord; // 顶点 shader 传过来的纹理坐标信息
         * uniform sampler2D texture; // 输入纹理
         */
        virtual const GLchar *fragmentShader();
        
        /**
         * shader 中的 attrib 和 uniform 变量在每次 process 都会赋值
         */
        virtual void getAttribLocations();
        virtual void resolveAttribLocations();
        
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
        virtual const GLfloat *positionVertices();
        virtual const GLfloat *texCoordForRotation(TexRotations rotation);
        
        virtual void updateTexture();
        
        virtual void draw();
        
    protected:
        std::shared_ptr<Framebuffer> _srcFramebuffer;
        std::shared_ptr<Framebuffer> _framebuffer;
        std::map<int, std::shared_ptr<Filter>> _outputs;
        
        /// 在设置 srcFramebuffer 时，读取 srcFramebuffer 的 width/height
        size_t _width;
        size_t _height;
        
        Program _program;
        
        GLuint _positionLocation;
        GLuint _texCoordLocation;
        GLuint _textureUniform;
        
        NoCopy(Filter);
        
    };
}

#endif /* TTFilter_hpp */
