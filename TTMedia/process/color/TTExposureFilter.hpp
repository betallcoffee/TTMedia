//
//  TTExposureFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 10/5/18.
//  Copyright © 2018年 tina. All rights reserved.
//  曝光

#ifndef TTExposureFilter_hpp
#define TTExposureFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class ExposureFilter : public Filter {
    public:
        ExposureFilter();
        ~ExposureFilter();
        
        /**
         Exposure ranges from -10.0 to 10.0, with 0.0 as the normal level
         */
        void setExposure(float exposure);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        float _exposure;
        GLint _exposureUniform;
        
        NoCopy(ExposureFilter);
    };
}

#endif /* TTExposureFilter_hpp */
