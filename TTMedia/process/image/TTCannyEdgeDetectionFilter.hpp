//
//  TTCannyEdgeDetectionFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 11/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

/** This applies the edge detection process described by John Canny in

Canny, J., A Computational Approach To Edge Detection, IEEE Trans. Pattern Analysis and Machine Intelligence, 8(6):679–698, 1986.

and implemented in OpenGL ES by

A. Ensor, S. Hall. GPU-based Image Analysis on Mobile Devices. Proceedings of Image and Vision Computing New Zealand 2011.

It starts with a conversion to luminance, followed by an accelerated 9-hit Gaussian blur. A Sobel operator is applied to obtain the overall
gradient strength in the blurred image, as well as the direction (in texture sampling steps) of the gradient. A non-maximum suppression filter
acts along the direction of the gradient, highlighting strong edges that pass the threshold and completely removing those that fail the lower
threshold. Finally, pixels from in-between these thresholds are either included in edges or rejected based on neighboring pixels.
*/

#ifndef TTCannyEdgeDetectionFilter_hpp
#define TTCannyEdgeDetectionFilter_hpp

#include "TTFilterGroup.hpp"
#include "TTGrayscaleFilter.hpp"
#include "TTSingleComponentGaussianBlurFilter.hpp"
#include "TTDirectionalSobelEdgeDetectionFilter.hpp"
#include "TTDirectionalNonmaximumSuppressionFilter.hpp"
#include "TTWeakPixelInclusionFilter.hpp"

namespace TT {

class CannyEdgeDetectionFilter : public FilterGroup {
public:
    CannyEdgeDetectionFilter();
    ~CannyEdgeDetectionFilter();
    
private:
    TT_SP(GrayscaleFilter) _grayscaleFilter = nullptr;
    TT_SP(SingleComponentGaussianBlurFilter) _singleComponentGaussianBlurFilter = nullptr;
    TT_SP(DirectionalSobelEdgeDetectionFilter) _sobelEdgeDetectionFilter = nullptr;
    TT_SP(DirectionalNonmaximumSuppressionFilter) _nonmaximumSuppressionFilter = nullptr;
    TT_SP(WeakPixelInclusionFilter) _weakPixelInclusionFilter = nullptr;
};

}

#endif /* TTCannyEdgeDetectionFilter_hpp */
