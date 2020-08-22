//
//  TTCGImagePicture.m
//  TTPlayerExample
//
//  Created by liang on 8/8/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#import <ImageIO/ImageIO.h>

#import "TTCGImagePicture.h"

@interface TTCGImagePicture()
{
    TT_SP(TT::Picture_ios) _picture;
}

@end

@implementation TTCGImagePicture

- (TT_SP(TT::Picture))picture
{
    // TODO no thread safe
    if (_picture == nullptr) {
        TT_SP(TT::Picture_ios) picture = TT_MK_SP(TT::Picture_ios)();
        if (picture) {
            picture->setObject(self);
        }
        _picture = picture;
    }
    return _picture;
}

- (BOOL)open
{
    TT_SP(TT::Picture) picture = [self picture];
    NSString *strUrl = [[NSString alloc] initWithCString:picture->url()->cStr()                                                encoding:NSUTF8StringEncoding];
    NSURL *nsUrl = [NSURL URLWithString:strUrl];
    CGImageSourceRef source = CGImageSourceCreateWithURL((CFURLRef)nsUrl, nil);
    CGImageRef cgImage = CGImageSourceCreateImageAtIndex(source, 0, nil);
    size_t width = CGImageGetWidth(cgImage);
    size_t height = CGImageGetHeight(cgImage);
    CFDataRef cfData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    const uint8_t *data = CFDataGetBytePtr(cfData);
    size_t dataSize = CFDataGetLength(cfData);
    TT_SP(TT::Frame) frame = TT_MK_SP(TT::Frame)(data, dataSize, TT::kMediaTypeVideo, TT::kTextureTypeRGB);
    frame->setwidth(width);
    frame->setheight(height);
    
    _picture->fillFrame(frame);
    CFRelease(cfData);
    
    return YES;
}

- (BOOL)close
{
    return NO;
}

@end

using namespace TT;

bool Picture_ios::open()
{
    CALL_SEL_RETURN(Picture, open);
}

bool Picture_ios::close()
{
    CALL_SEL_RETURN(Picture, close);
}

void Picture_ios::fillFrame(TT_SP(Frame) frame)
{
    setframe(frame);
}


