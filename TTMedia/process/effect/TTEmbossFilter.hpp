//
//  TTEmbossFilter.hpp
//  TTMedia
//
//  Created by liang on 19/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//
/**
浮雕效果
*/

#ifndef TTEmbossFilter_hpp
#define TTEmbossFilter_hpp

#include "TTConvolution3x3Filter.hpp"

namespace TT {

class EmbossFilter : public Convolution3x3Filter {
public:
    EmbossFilter();
    ~EmbossFilter();
    
    // The strength of the embossing, from  0.0 to 4.0, with 1.0 as the normal level
    TT_PROPERTY_DECL(float, intensity);
    
};

}

#endif /* TTEmbossFilter_hpp */
