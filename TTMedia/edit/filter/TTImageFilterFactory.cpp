//
//  TTImageFilterFactory.cpp
//  TTPlayerExample
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTImageFilterFactory.hpp"
#include "TTProcess.h"

using namespace TT;

int32_t ImageFilterFactory::filterCount()
{
    return 2;
}

std::string ImageFilterFactory::filterName(int32_t index)
{
    switch (index) {
        case 0:
            return "CannyEdgeDetection";
        case 1:
            return "Beautify";
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
        default:
            break;
    }
    
    return nullptr;
}

TT_SP(Filter) ImageFilterFactory::createBeautifyFilter()
{
    TT_SP(TT::FilterGroup) cannyEdgeDetectionFilter = TT_MK_SP(TT::CannyEdgeDetectionFilter)();
    cannyEdgeDetectionFilter->assembleFilters();
    
    return cannyEdgeDetectionFilter;
}

TT_SP(Filter) ImageFilterFactory::createCannyEdgeDetectionFilter()
{
    TT_SP(TT::FilterGroup) beautifyFilter = TT_MK_SP(TT::BeautifyFilter)();
    beautifyFilter->assembleFilters();
    
    return beautifyFilter;
}
