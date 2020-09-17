//
//  TTBuffingFilter.hpp
//  TTMedia
//
//  Created by liang on 18/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTBuffingFilter_hpp
#define TTBuffingFilter_hpp

#include "TTThreeInputFilter.hpp"

namespace TT {

/**
 * 磨皮滤镜，使用 canney 边缘检测和肤色检测对皮肤基于双边滤波进行模糊
 * 输入 1 原图
 * 输入 2 canny
 * 输入 3 bilateral
 */
class BuffingFilter : public ThreeInputFilter {
public:
    BuffingFilter();
    ~BuffingFilter();
    
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
    const GLchar *fragmentShader() override;
    
    /**
     * shader 中的 attrib 和 uniform 变量在每次 process 都会赋值
     */
    void getUniformLocations() override;
    void resolveUniformLocations() override;
    
    void setSmoothDegree(float smoothDegree) { _smoothDegree = smoothDegree; }
private:
    float _smoothDegree = 0.5;
    GLint _smoothDegreeUniform = 0;
};

}

#endif /* TTBuffingFilter_hpp */
