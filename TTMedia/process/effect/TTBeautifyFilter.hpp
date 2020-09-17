//
//  TTBeautifyFilter.hpp
//  TTMedia
//
//  Created by liang on 18/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTBeautifyFilter_hpp
#define TTBeautifyFilter_hpp

#include "TTFilterGroup.hpp"
#include "TTBuffingFilter.hpp"
#include "TTBilateralFilter.hpp"
#include "TTCannyEdgeDetectionFilter.hpp"

namespace TT {

class BeautifyFilter : public FilterGroup {
public:
    BeautifyFilter();
    ~BeautifyFilter();
    
    void assembleFilters() override;
    
private:
    TT_SP(BilateralFilter) _bilateralFilter = nullptr;
    TT_SP(CannyEdgeDetectionFilter) _cannyEdgeDetectionFilter = nullptr;
    TT_SP(BuffingFilter) _buffingFilter = nullptr;
};

}

#endif /* TTBeautifyFilter_hpp */
