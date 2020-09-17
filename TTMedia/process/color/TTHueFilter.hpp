//
//  TTHueFilter.hpp
//  TTMedia
//
//  Created by liang on 12/5/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTHueFilter_hpp
#define TTHueFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class HueFilter : public Filter {
    public:
        HueFilter();
        ~HueFilter();
        
        void setHueAdjust(float hueAdjust);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        float _hueAdjust;
        GLint _hueAdjustUniform;
        
        NoCopy(HueFilter);
    };
}

#endif /* TTHueFilter_hpp */
