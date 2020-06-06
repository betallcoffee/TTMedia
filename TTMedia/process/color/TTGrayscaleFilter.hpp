//
//  TTGrayscaleFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 6/6/2020.
//  Copyright © 2020 tina. All rights reserved.
// 灰度

/** Converts an image to grayscale (a slightly faster implementation of the saturation filter, without the ability to vary the color contribution)
*/

#ifndef TTGrayscaleFilter_hpp
#define TTGrayscaleFilter_hpp

#include "TTFilter.hpp"

namespace TT {

    class GrayscaleFilter : public Filter {
    public:
        GrayscaleFilter();
        ~GrayscaleFilter();
    
    private:
        virtual const GLchar *fragmentShader() override;
    };

}

#endif /* TTGrayscaleFilter_hpp */
