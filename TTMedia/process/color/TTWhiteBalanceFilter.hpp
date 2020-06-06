//
//  TTWhiteBalanceFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 12/5/18.
//  Copyright © 2018年 tina. All rights reserved.
// 白平衡

/**
 * Created by Alaric Cole
 * Allows adjustment of color temperature in terms of what an image was effectively shot in. This means higher Kelvin values will warm the image, while lower values will cool it.
 */

#ifndef TTWhiteBalanceFilter_hpp
#define TTWhiteBalanceFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class WhiteBalanceFilter : public Filter {
    public:
        WhiteBalanceFilter();
        ~WhiteBalanceFilter();
        
        void setTemperature(GLfloat temperature);
        void setTint(GLfloat tint);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        GLfloat _temperature;
        GLfloat _tint;
        GLint _temperatureUniform, _tintUniform;
    };
}

#endif /* TTWhiteBalanceFilter_hpp */
