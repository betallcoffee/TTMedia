//
//  TTLevelFilter.hpp
//  TTMedia
//
//  Created by liang on 12/5/18.
//  Copyright © 2018年 tina. All rights reserved.
//

/**
 * Levels like Photoshop.
 *
 * The min, max, minOut and maxOut parameters are floats in the range [0, 1].
 * If you have parameters from Photoshop in the range [0, 255] you must first
 * convert them to be [0, 1].
 * The gamma/mid parameter is a float >= 0. This matches the value from Photoshop.
 *
 * If you want to apply levels to RGB as well as individual channels you need to use
 * this filter twice - first for the individual channels and then for all channels.
 */

#ifndef TTLevelFilter_hpp
#define TTLevelFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class LevelFilter : public Filter {
    public:
        LevelFilter();
        ~LevelFilter();
        
        /** Set levels for the red channel */
        void setRedLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut);
        void setRedLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn);
        
        /** Set levels for the green channel */
        void setGreenLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut);
        void setGreenLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn);
        
        /** Set levels for the blue channel */
        void setBlueLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut);
        void setBlueLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn);
        
        /** Set levels for all channels at once */
        void setLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn, GLfloat minOut, GLfloat maxOut);
        void setLevels(GLfloat minIn, GLfloat gamma, GLfloat maxIn);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        GLint _minInUniform;
        GLint _gammaUniform;
        GLint _maxInUniform;
        GLint _minOutputUniform;
        GLint _maxOutputUniform;
        
        GPUVector3 _minInVector, _gammaVector, _maxInVector, _minOutputVector, _maxOutputVector;
    };
}

#endif /* TTLevelFilter_hpp */
