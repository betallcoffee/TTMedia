//
//  TTPlayer_ios.m
//  TTMedia
//
//  Created by liang on 19/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "TTPlayer_ios.h"
#include "TTAudioQueue_ios.hpp"

static bool renderSetup(void *opaque, TT::Render *render) {
    if (opaque == nullptr || render == nullptr) {
        return false;
    }
    
    TTOpenGLView *view = (__bridge TTOpenGLView *)opaque;
    return [view setupRender:render];
}

static void renderTeardown(void *opaque) {
    
}

static bool renderDisplay(void *opaque, std::shared_ptr<TT::Frame> frame) {
    if (opaque == nullptr) {
        return false;
    }
    
    TTOpenGLView *view = (__bridge TTOpenGLView *)opaque;
    return [view render:frame];
}

std::shared_ptr<TT::Player> createPlayer_ios() {
    std::shared_ptr<TT::Player> player = std::make_shared<TT::Player>();
    if (player != nullptr) {
        player->init();
        player->bindAudioQueue(std::make_shared<TT::AudioQueue_ios>());
    }
    return player;
}

BOOL bindGLView_ios(std::shared_ptr<TT::Player> player, TTOpenGLView *view) {
    if (player == nullptr || view == nil) {
        return NO;
    }
    
    std::shared_ptr<TT::RenderContext> ctx = std::make_shared<TT::RenderContext>();
    ctx->opaque = (__bridge void *)view;
    ctx->setup = renderSetup;
    ctx->teardown = renderTeardown;
    ctx->display = renderDisplay;
    
    player->bindRenderContext(ctx);
    
    return YES;
}
