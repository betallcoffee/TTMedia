//
//  TTCannyEdgeDetectionFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTCannyEdgeDetectionFilter.hpp"

using namespace TT;

CannyEdgeDetectionFilter::CannyEdgeDetectionFilter()
{
}

CannyEdgeDetectionFilter::~CannyEdgeDetectionFilter()
{
    
}

void CannyEdgeDetectionFilter::assembleFilters()
{
    // 1. 灰度图
    _grayscaleFilter = TT_MK_SP(GrayscaleFilter)();
    // 2. 高斯模糊
    _singleComponentGaussianBlurFilter = TT_MK_SP(SingleComponentGaussianBlurFilter)();
    _singleComponentGaussianBlurFilter->setBlurRadius(4.0);
    _singleComponentGaussianBlurFilter->setTexelSpacingMultiplier(1.0);
    // 3. sobel 边缘检测
    _sobelEdgeDetectionFilter = TT_MK_SP(DirectionalSobelEdgeDetectionFilter)();
    // 4. 非最大值抑制，使用上下阀值来检测边缘
    _nonmaximumSuppressionFilter = TT_MK_SP(DirectionalNonmaximumSuppressionFilter)();
    _nonmaximumSuppressionFilter->setUpperThreshold(0.4);
    _nonmaximumSuppressionFilter->setLowerThreshold(0.1);
    // 5. 抑制孤立的弱边缘
    _weakPixelInclusionFilter = TT_MK_SP(WeakPixelInclusionFilter)();
    
    _grayscaleFilter->addOutput(_singleComponentGaussianBlurFilter);
    _singleComponentGaussianBlurFilter->addOutput(_sobelEdgeDetectionFilter);
    _sobelEdgeDetectionFilter->addOutput(_nonmaximumSuppressionFilter);
    _nonmaximumSuppressionFilter->addOutput(_weakPixelInclusionFilter);
    
    addFilter(_grayscaleFilter);
    _sinkFilter = _weakPixelInclusionFilter;
}
