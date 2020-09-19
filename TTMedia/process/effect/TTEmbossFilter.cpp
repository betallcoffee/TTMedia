//
//  TTEmbossFilter.cpp
//  TTMedia
//
//  Created by liang on 19/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTEmbossFilter.hpp"

using namespace TT;

EmbossFilter::EmbossFilter()
{
    setintensity(1.0);
}

EmbossFilter::~EmbossFilter()
{
    
}

float EmbossFilter::intensity()
{
    return _intensity;
}

void EmbossFilter::setintensity(float intensity)
{
    _intensity = intensity;
    
    GPUMatrix3x3 newConvolutionKernel;
    newConvolutionKernel.one.one = _intensity * (-2.0);
    newConvolutionKernel.one.two = -_intensity;
    newConvolutionKernel.one.three = 0.0f;

    newConvolutionKernel.two.one = -_intensity;
    newConvolutionKernel.two.two = 1.0;
    newConvolutionKernel.two.three = _intensity;
    
    newConvolutionKernel.three.one = 0.0f;
    newConvolutionKernel.three.two = _intensity;
    newConvolutionKernel.three.three = _intensity * 2.0;
    
    setconvolutionKernel(newConvolutionKernel);
}
