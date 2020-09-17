//
//  TTImageFilterFactory.hpp
//  TTMedia
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTImageFilterFactory_hpp
#define TTImageFilterFactory_hpp

#include <string.h>
#include "TTDef.h"
#include "TTFilter.hpp"

namespace TT {

class ImageFilterFactory {
public:
    ImageFilterFactory() {};
    ~ImageFilterFactory() {};
    
    static int32_t filterCount();
    static std::string filterName(int32_t index);
    static TT_SP(Filter) createFilter(int32_t index);
    
private:
    static TT_SP(Filter) createBeautifyFilter();
    static TT_SP(Filter) createCannyEdgeDetectionFilter();
};

}

#endif /* TTImageFilterFactory_hpp */
