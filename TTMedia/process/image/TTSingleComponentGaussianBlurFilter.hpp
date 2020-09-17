//
//  TTSingleComponentGaussianBlurFilter.hpp
//  TTMedia
//
//  Created by liang on 6/6/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTSingleComponentGaussianBlurFilter_hpp
#define TTSingleComponentGaussianBlurFilter_hpp

#include <vector>
#include "TTGaussianBlurFilter.hpp"

namespace TT {

class SingleComponentGaussianBlurFilter : public GaussianBlurFilter {
public:
    SingleComponentGaussianBlurFilter();
    ~SingleComponentGaussianBlurFilter();
    
private:
    void createGaussianWeights();
    
    virtual void createVertexShader() override;
    virtual void createFragmentShader() override;

private:
    std::vector<float> _standardGaussianWeights;
    
    int32_t _numberOfOptimizedOffsets = 0;
    std::vector<float> _optimizedGaussianWeights;
    std::vector<float> _optimizedGaussianOffsets;
};

}

#endif /* TTSingleComponentGaussianBlurFilter_hpp */
