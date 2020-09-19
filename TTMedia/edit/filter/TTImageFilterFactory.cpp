//
//  TTImageFilterFactory.cpp
//  TTMedia
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTImageFilterFactory.hpp"
#include "TTProcess.h"

using namespace TT;

int32_t ImageFilterFactory::filterCount()
{
    return 3;
}

std::string ImageFilterFactory::filterName(int32_t index)
{
    switch (index) {
        case 0:
            return "CannyEdgeDetection";
        case 1:
            return "Beautify";
        case 2:
            return "Emboss";
        default:
            break;
    }
    
    return "unknow";
}

TT_SP(Filter) ImageFilterFactory::createFilter(int32_t index)
{
    switch (index) {
        case 0:
            return createCannyEdgeDetectionFilter();
        case 1:
            return createBeautifyFilter();
        case 2:
            return createEmbossFilter();
        default:
            break;
    }
    
    return nullptr;
}

TT_SP(Filter) ImageFilterFactory::createBeautifyFilter()
{
    TT_SP(FilterGroup) cannyEdgeDetectionFilter = TT_MK_SP(CannyEdgeDetectionFilter)();
    cannyEdgeDetectionFilter->assembleFilters();
    
    return cannyEdgeDetectionFilter;
}

TT_SP(Filter) ImageFilterFactory::createCannyEdgeDetectionFilter()
{
    TT_SP(FilterGroup) beautifyFilter = TT_MK_SP(BeautifyFilter)();
    beautifyFilter->assembleFilters();
    
    return beautifyFilter;
}

TT_SP(Filter) ImageFilterFactory::createEmbossFilter()
{
    TT_SP(EmbossFilter) embossFilter = TT_MK_SP(EmbossFilter)();
    embossFilter->setintensity(4.0);
    return embossFilter;
}
