//
//  TTBeautifyFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 18/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTBeautifyFilter.hpp"

using namespace TT;

BeautifyFilter::BeautifyFilter()
{
    
}

BeautifyFilter::~BeautifyFilter()
{
    
}

void BeautifyFilter::assembleFilters()
{
    // First pass: edge detection
    _cannyEdgeDetectionFilter = TT_MK_SP(CannyEdgeDetectionFilter)();
    _cannyEdgeDetectionFilter->assembleFilters();
    addFilter(_cannyEdgeDetectionFilter, 2);
    
    // Second pass: face smoothing filter
    _bilateralFilter = TT_MK_SP(BilateralFilter)();
    addFilter(_bilateralFilter, 3);
    
    // Third pass: combination bilateral, edge detection and origin
    _buffingFilter = TT_MK_SP(BuffingFilter)();
    addFilter(_buffingFilter, 1);
    
    _cannyEdgeDetectionFilter->addOutput(_buffingFilter, 2);
    _bilateralFilter->addOutput(_buffingFilter, 3);
    _sinkFilter = _buffingFilter;
}
