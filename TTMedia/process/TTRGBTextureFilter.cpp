//
//  TTRGBTextureFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 23/8/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "TTRGBTextureFilter.hpp"
#include "TTFramebuffer.hpp"

using namespace TT;

void RGBTextureFilter::updateTexture()
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _outputFramebuffer->textrue());
    
    // Using BGRA extension to pull in video frame data directly
    // The use of bytesPerRow / 4 accounts for a display glitch present in preview video frames when using the photo preset on the camera
    size_t bytesPerRow = 4 * _width;
    const void *bytes = _frame->data[0];
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bytesPerRow / 4, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
}

void RGBTextureFilter::processFrame(TT_SP(Frame) frame)
{
    if (frame) {
        _frame = frame;
        _width = _frame->width();
        _height = _frame->height();
        process(frame->pts);
        _frame = nullptr;
    }
}
