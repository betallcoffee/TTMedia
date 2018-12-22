//
//  TTVideoStateM.cpp
//  TTPlayerExample
//
//  Created by liang on 16/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTVideoStateM.hpp"
#include "TTVideo.hpp"

using namespace TT;

VideoState::VideoState(std::shared_ptr<Video> video)
: _video(video)
{
    
}

VideoStateInit::VideoStateInit(std::shared_ptr<Video> video)
: VideoState(video)
{
    
}

bool VideoStateInit::enter() {
    std::shared_ptr<Video> v = video().lock();
    if (v) {
        v->closeMedia();
    }
    return true;
}

bool VideoStateInit::leave() {
    return true;
}

std::shared_ptr<State> VideoStateInit::slot(int signal) {
    VideoStateEvent videoEvent = static_cast<VideoStateEvent>(signal);
    if (videoEvent == VideoStateEvent::kOpen) {
        return std::make_shared<VideoStateRead>(video().lock());
    }
    
    return nullptr;
}

int VideoStateInit::run() {
    return static_cast<int>(VideoStateEvent::kNone);
}

VideoStateRead::VideoStateRead(std::shared_ptr<Video> video)
: VideoState(video)
{
    
}

bool VideoStateRead::enter() {
    std::shared_ptr<Video> v = video().lock();
    if (v) {
        v->openDemuxer();
    }
    return true;
}

bool VideoStateRead::leave() {
    return true;
}

std::shared_ptr<State> VideoStateRead::slot(int signal) {
    VideoStateEvent videoEvent = static_cast<VideoStateEvent>(signal);
    if (videoEvent == VideoStateEvent::kReady) {
        return std::make_shared<VideoStateEdit>(video().lock());
    } else if (videoEvent == VideoStateEvent::kClose) {
        return std::make_shared<VideoStateInit>(video().lock());
    }
    
    return nullptr;
}

int VideoStateRead::run() {
    std::shared_ptr<Video> v = video().lock();
    if (v) {
        return v->readData();
    }
    return static_cast<int>(VideoStateEvent::kNone);
}

VideoStateEdit::VideoStateEdit(std::shared_ptr<Video> video)
: VideoState(video)
{
    
}

bool VideoStateEdit::enter() {
    return true;
}

bool VideoStateEdit::leave() {
    return true;
}

std::shared_ptr<State> VideoStateEdit::slot(int signal) {
    VideoStateEvent videoEvent = static_cast<VideoStateEvent>(signal);
    if (videoEvent == VideoStateEvent::kSave) {
        return std::make_shared<VideoStateWrite>(video().lock());
    } else if (videoEvent == VideoStateEvent::kClose) {
        return std::make_shared<VideoStateInit>(video().lock());
    }
    
    return nullptr;
}

int VideoStateEdit::run() {
    return static_cast<int>(VideoStateEvent::kNone);
}

VideoStateWrite::VideoStateWrite(std::shared_ptr<Video> video)
: VideoState(video)
{
    
}

bool VideoStateWrite::enter() {
    return true;
}

bool VideoStateWrite::leave() {
    return true;
}

std::shared_ptr<State> VideoStateWrite::slot(int signal) {
    VideoStateEvent videoEvent = static_cast<VideoStateEvent>(signal);
    if (videoEvent == VideoStateEvent::kReady) {
        return std::make_shared<VideoStateEdit>(video().lock());
    } else if (videoEvent == VideoStateEvent::kClose) {
        return std::make_shared<VideoStateInit>(video().lock());
    }
    
    return nullptr;
}

int VideoStateWrite::run() {
    std::shared_ptr<Video> v = video().lock();
    if (v) {
        return v->writeData();
    }
    return static_cast<int>(VideoStateEvent::kNone);
}

VideoStateM::VideoStateM(std::shared_ptr<Video> video)
: _video(video)
{
    std::shared_ptr<VideoState> state = std::make_shared<VideoStateInit>(video);
    init(state);
}

VideoStateM::~VideoStateM() {
    
}
