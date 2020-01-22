//
//  TTFilterCommon.h
//  TTMedia
//
//  Created by liang on 22/1/20.
//  Copyright © 2020年 tina. All rights reserved.
//

#ifndef TTFilterCommon_h
#define TTFilterCommon_h

namespace TT {
    
    struct GPUVector4 {
        GLfloat one;
        GLfloat two;
        GLfloat three;
        GLfloat four;
    };
    typedef struct GPUVector4 GPUVector4;
    
    struct GPUVector3 {
        GLfloat one;
        GLfloat two;
        GLfloat three;
    };
    typedef struct GPUVector3 GPUVector3;
    
    struct GPUMatrix4x4 {
        GPUVector4 one;
        GPUVector4 two;
        GPUVector4 three;
        GPUVector4 four;
    };
    typedef struct GPUMatrix4x4 GPUMatrix4x4;
    
    struct GPUMatrix3x3 {
        GPUVector3 one;
        GPUVector3 two;
        GPUVector3 three;
    };
    typedef struct GPUMatrix3x3 GPUMatrix3x3;
}

#endif /* TTFilterCommon_h */
