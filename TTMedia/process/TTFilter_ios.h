//
//  TTFilter_ios.h
//  TTPlayerExample
//
//  Created by liang on 20/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

/**
 * Filter_ios 将 Filter pipe 的调用转发到 oc 对象
 */


#import <Foundation/Foundation.h>

#include "TTDefForObjc.h"
#include "TTFilter.hpp"
#include "TTClass.hpp"

@protocol TTFilterDelegate <NSObject>

@required
- (std::shared_ptr<TT::Filter>)filter;

@optional
- (BOOL)bindFramebuffer;

- (void)compileShader;
- (const GLchar *)vertexShader;
- (void)getAttribLocations;
- (void)resolveAttribLocations;

- (const GLchar *)fragmentShader;
- (void)getUniformLocations;
- (void)resolveUniformLocations;

- (const GLfloat *)positionVertices;
- (const GLfloat *)texCoordForRotation:(TT::TexRotations)rotation;

- (void)updateTexture;
- (void)draw;
- (void)notifyFramebufferToFilters:(int64_t)timestamp;

@end

@interface TTFilter : NSObject <TTFilterDelegate>

@end

#ifndef TTFilter_ios_hpp
#define TTFilter_ios_hpp

namespace TT {
    
    class Filter_ios : public Filter {
    public:
        Filter_ios() {};
        virtual ~Filter_ios() {};
        
        OBJC_DELEGATE(TTFilterDelegate)
    protected:
        virtual bool bindFramebuffer();
        
        virtual void compileShader();
        virtual const GLchar *vertexShader();
        virtual void getAttribLocations();
        virtual void resolveAttribLocations();
        
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
        virtual const GLfloat *positionVertices();
        virtual const GLfloat *texCoordForRotation(TexRotations rotation);
        
        virtual void updateTexture();
        
        virtual void draw();
        
        virtual void notifyFramebufferToFilters(int64_t timestamp);
        
    protected:
        NoCopy(Filter_ios);
        
    };
}

#endif // TTFilter_ios_hpp
