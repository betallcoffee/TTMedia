//
//  TTCGImagePicture.h
//  TTPlayerExample
//
//  Created by liang on 8/8/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTCGImagePicture_h
#define TTCGImagePicture_h

#include <Foundation/Foundation.h>

#include "TTDef.h"
#include "TTDefForObjc.h"
#include "TTPicture.hpp"
#include "TTURL.hpp"


@protocol TTPictureDelegate <NSObject>

@required
- (TT_SP(TT::Picture))picture;

- (BOOL)open;
- (BOOL)close;

@end

@interface TTCGImagePicture : NSObject<TTPictureDelegate>

- (TT_SP(TT::Picture))picture;

- (BOOL)open;
- (BOOL)close;

@end

namespace TT {

class Picture_ios : public Picture {
public:
    Picture_ios() {};
    ~Picture_ios() {};
    
    bool open() override;
    bool close() override;
    
    OBJC_DELEGATE(TTPictureDelegate);
    
public:
    void fillFrame(TT_SP(Frame) frame);
    
};

}


#endif /* TTCGImagePicture_h */
