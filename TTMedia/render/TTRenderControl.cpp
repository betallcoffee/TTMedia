//
//  TTRenderControl.cpp
//  TTPlayerExample
//
//  Created by liang on 2/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include <unistd.h>

#include "easylogging++.h"

#include "TTFilter.hpp"

#include "TTRenderControl.hpp"
#include "TTMessageLoop.hpp"

#include "TTFrame.hpp"

using namespace TT;

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 40
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 100
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 150
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10000.0

RenderControl::RenderControl() {
    _vPTS = 0;
    _aClock.reset();
    _vClock.reset();
    _eClock.reset();
    
    _loop = std::make_shared<MessageLoop>();
    _loop->setMessageHandle(std::bind(&RenderControl::handleMessage, this, std::placeholders::_1));
    initMessages();
    _loop->start();
}

RenderControl::~RenderControl() {
    _loop->stop();
}

bool RenderControl::start() {
    _loop->emitMessage(kRenderRender);
    return true;
}

bool RenderControl::stop() {
    return false;
}

void RenderControl::updateAudioClock(int64_t pts) {
    _aClock.setClock(pts);
}

void RenderControl::initMessages() {
    _loop->signalMessage(std::make_shared<Message>(kRenderRender, [&](std::shared_ptr<Message> message) {
        this->render();
    }));
}

void RenderControl::handleMessage(std::shared_ptr<Message> message) {
    
}

void RenderControl::render() {
    std::shared_ptr<Frame> frame = _frameQueue->pop();
    if (frame) {
        int64_t delay = frame->pts - _vPTS;
        LOG(TRACE) << "render delay1 " << delay;
        /* update delay to follow master synchronisation source */
        if (_clock != AV_SYNC_VIDEO_MASTER) {
            /* if video is slave, we try to correct big delays by
             duplicating or deleting a frame */
            int64_t diff = _vClock.getClock() - getMasterClock();
            LOG(TRACE) << "render diff " << diff;
            /* skip or repeat frame. We take into account the
             delay to compute the threshold. I still don't know
             if it is the best guess */
            int64_t syncThreshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
            LOG(TRACE) << "sync threshold " << syncThreshold;
            if (!isnan(diff) && abs(diff) < AV_NOSYNC_THRESHOLD) {
                if (diff <= -syncThreshold)
                    delay = FFMAX(0, delay + diff);
                else if (diff >= syncThreshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                    delay = delay + diff;
                else if (diff >= syncThreshold)
                    delay = 2 * delay;
            } else {
                LOG(WARNING) << "diff is lager " << diff;
            }
        }
        
        LOG(TRACE) << "render video frame " << frame->pts;
        //                    _render.displayFrame(frame);
        _filter->processFrame(frame);
        _vPTS = frame->pts;
        _vClock.setClock(frame->pts);
        LOG(TRACE) << "render delay2 " << delay;
        if (delay < 0) {
            LOG(WARNING) << "delay is negative " << delay;
            delay = 0;
        }
        usleep(delay * 1000);
    }
    _loop->emitMessage(kRenderRender);
}

double RenderControl::getMasterClock() {
    double val;
    switch (_clock) {
        case AV_SYNC_VIDEO_MASTER:
            val = _vClock.getClock();
            break;
        case AV_SYNC_AUDIO_MASTER:
            val = _aClock.getClock();
            break;
        default:
            val = _eClock.getClock();
            break;
    }
    return val;
}

