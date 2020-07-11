//
//  TTDirectionalNonmaximumSuppressionFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTDirectionalNonmaximumSuppressionFilter_hpp
#define TTDirectionalNonmaximumSuppressionFilter_hpp

#include "TTFilter.hpp"

namespace TT {

class DirectionalNonmaximumSuppressionFilter : public Filter {
public:
    DirectionalNonmaximumSuppressionFilter();
    ~DirectionalNonmaximumSuppressionFilter();
    
    virtual void setupFilterSize() override;
    /**
     *返回自定义的片段 shader
     *其中约定 ：
     * varying highp vec2 v_texcoord; // 顶点 shader 传过来的纹理坐标信息
     * uniform sampler2D texture; // 输入纹理
     */
    virtual const GLchar *fragmentShader() override;
    
    virtual void getUniformLocations() override;
    virtual void resolveUniformLocations() override;
    
private:
    // The texel width and height determines how far out to sample from this texel. By default, this is the normalized width of a pixel, but this can be overridden for different effects.
    
    float _texelWidth = 0;
    float _texelHeight = 0;
    GLint _texelWidthUniform = 0;
    GLint _texelHeightUniform = 0;

    // These thresholds set cutoffs for the intensities that definitely get registered (upper threshold) and those that definitely don't (lower threshold)
    float _upperThreshold = 0.5;
    float _lowerThreshold = 0.1;
    GLint _upperThresholdUniform = 0;
    GLint _lowerThresholdUniform = 0;
    
};

}

#endif /* TTDirectionalNonmaximumSuppressionFilter_hpp */
