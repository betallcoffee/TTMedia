//
//  TTFilter_ios.m
//  TTMedia
//
//  Created by liang on 20/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTFilter_ios.h"
#import "TTDefForObjc.h"

@interface TTFilter ()
{
    std::shared_ptr<TT::Filter> _filter;
}

@end

@implementation TTFilter

- (void)dealloc {
    if (_filter) {
        _filter.reset();
    }
}

- (std::shared_ptr<TT::Filter>)filter {
    // TODO no thread safe
    if (_filter == nullptr) {
        std::shared_ptr<TT::Filter_ios> filter = std::make_shared<TT::Filter_ios>();
        if (filter) {
            filter->setObject(self);
        }
        _filter = filter;
    }
    return _filter;
}

@end

using namespace TT;

bool Filter_ios::bindFramebuffer() {
    CALL_SEL_RETURN(Filter, bindFramebuffer);
}

void Filter_ios::compileShader() {
    CALL_SEL(Filter, compileShader);
}

const GLchar *Filter_ios::vertexShader() {
    CALL_SEL_RETURN(Filter, vertexShader);
}

const GLchar *Filter_ios::fragmentShader() {
    CALL_SEL_RETURN(Filter, fragmentShader);
}

void Filter_ios::getAttribLocations() {
    CALL_SEL(Filter, getAttribLocations);
}

void Filter_ios::getUniformLocations() {
    CALL_SEL(Filter, getUniformLocations);
}

void Filter_ios::resolveAttribLocations() {
    CALL_SEL(Filter, resolveAttribLocations);
}

void Filter_ios::resolveUniformLocations() {
    CALL_SEL(Filter, resolveUniformLocations);
}

const GLfloat *Filter_ios::positionVertices() {
    CALL_SEL_RETURN(Filter, positionVertices);
}

const GLfloat *Filter_ios::texCoordForRotation(TexRotations rotation) {
    CALL_SEL_RETURN_1(Filter, texCoordForRotation, rotation);
}

void Filter_ios::updateTexture() {
    CALL_SEL(Filter, updateTexture);
}

void Filter_ios::draw() {
    CALL_SEL(Filter, draw);
}

void Filter_ios::notifyFramebufferToFilters(int64_t timestamp) {
    CALL_SEL_1(Filter, notifyFramebufferToFilters, timestamp);
}


