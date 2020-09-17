//
//  TTPlayer_ios.h
//  TTMedia
//
//  Created by liang on 19/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTPlayer_ios_h
#define TTPlayer_ios_h

#include "TTPlayer.hpp"
#include "TTRender.hpp"

#import "TTOpenGLView.h"

std::shared_ptr<TT::Player> createPlayer_ios();

BOOL bindGLView_ios(std::shared_ptr<TT::Player> player, TTOpenGLView *view);


#endif /* TTPlayer_ios_h */
