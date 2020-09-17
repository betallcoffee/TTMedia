//
//  TTDirectionalSobelEdgeDetectionFilter.hpp
//  TTMedia
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTDirectionalSobelEdgeDetectionFilter_hpp
#define TTDirectionalSobelEdgeDetectionFilter_hpp

#include "TTTexture3x3SamplingFilter.hpp"

namespace TT {

class DirectionalSobelEdgeDetectionFilter : public Texture3x3SamplingFilter {
public:
    DirectionalSobelEdgeDetectionFilter();
    ~DirectionalSobelEdgeDetectionFilter();
    
    /**
     *返回自定义的片段 shader
     *其中约定 ：
     * varying highp vec2 v_texcoord; // 顶点 shader 传过来的纹理坐标信息
     * uniform sampler2D texture; // 输入纹理
     *
     * varying vec2 v_textureCoordinate;
     * varying vec2 v_leftTextureCoordinate;
     * varying vec2 v_rightTextureCoordinate;
     *
     * varying vec2 v_topTextureCoordinate;
     * varying vec2 v_topLeftTextureCoordinate;
     * varying vec2 v_topRightTextureCoordinate;
     *
     * varying vec2 v_bottomTextureCoordinate;
     * varying vec2 v_bottomLeftTextureCoordinate;
     * varying vec2 v_bottomRightTextureCoordinate;
     */
    virtual const GLchar *fragmentShader() override;
};

}

#endif /* TTDirectionalSobelEdgeDetectionFilter_hpp */
