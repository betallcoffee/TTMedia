//
//  TTPlayer.cpp
//  TTPlayerExample
//
//  Created by liang on 8/21/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <stdlib.h>

#include "easylogging++.h"

#include "TTPlayer.hpp"

#include "TTMutex.hpp"
#include "TTURL.hpp"

#include "TTHTTPIO.hpp"
#include "TTAudioCodec.hpp"

#include "TTDemuxer.hpp"
#include "TTDemuxerControl.hpp"
#include "TTCodecControl.hpp"

#include "TTFilter.hpp"

using namespace TT;

INITIALIZE_EASYLOGGINGPP

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 40
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 100
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 150
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10000.0

Player::Player() : _status(kPlayerNone),
 _statusCond(PTHREAD_COND_INITIALIZER), _statusMutex(PTHREAD_MUTEX_INITIALIZER),
 _inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
 _renderCond(PTHREAD_COND_INITIALIZER), _renderMutex(PTHREAD_MUTEX_INITIALIZER), _renderring(false),
 _clock(AV_SYNC_AUDIO_MASTER) {
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
     pthread_create(&_inputThread, nullptr, Player::inputThreadEntry, this);
     pthread_create(&_renderThread, nullptr, Player::renderThreadEntry, this);
}

Player::~Player() {
    close();
}

void Player::bindRenderContext(const RenderContext &context) {
    _render.bindContext(context);
}

void Player::bindAudioQueue(std::shared_ptr<AudioQueue> audioQueue) {
    _audioQueue = audioQueue;
    if (_audioQueue) {
        _audioQueue->setQueueCallback(std::bind(&Player::audioQueueCB, this));
    }
}

void Player::bindFilter(std::shared_ptr<Filter> filter) {
    _filter.addFilter(filter);
}

void Player::play(std::shared_ptr<URL> url) {
    if (_status == kPlayerNone ||
        _status == kPlayerStoped) {
        _url = url;
        setStatus(kPlayerOpen);
    }
}

void Player::stop() {
    setStatus(kPlayerClose);
}

void Player::pause() {
    setStatus(kPlayerPaused);
}

void Player::resume() {
    setStatus(kPlayerPlaying);
}

void Player::setStatus(ePlayerStatus status) {
    Mutex m(&_statusMutex);
    LOG(DEBUG) << "Change status " << _status << " to " << status;
    switch (status) {
        case kPlayerOpen:
        {
            if (_status == kPlayerNone ||
                _status == kPlayerError ||
                _status == kPlayerStoped) {
                _status = kPlayerOpen;
            }
            break;
        }
        case kPlayerClose:
        {
            if (_status == kPlayerError ||
                _status == kPlayerOpen ||
                _status == kPlayerPlaying ||
                _status == kPlayerPaused) {
                _status = kPlayerClose;
            }
            break;
        }
        case kPlayerPlaying:
        {
            if (_status == kPlayerOpen ||
                _status == kPlayerPaused) {
                _status = kPlayerPlaying;
            }
            break;
        }
        case kPlayerPaused:
        {
            if (_status == kPlayerPlaying) {
                _status = kPlayerPaused;
            }
            break;
        }
        case kPlayerStoped:
        {
            if (_status == kPlayerClose) {
                _status = kPlayerStoped;
            }
            break;
        }
        case kPlayerQuit:
        {
            _status = kPlayerQuit;
            break;
        }
            
        default:
            break;
    }
    pthread_cond_broadcast(&_statusCond);
}

void Player::waitStatusChange() {
    Mutex m(&_statusMutex);
    pthread_cond_wait(&_statusCond, &_statusMutex);
}

bool Player::open() {
    if (_status == kPlayerOpen) {        
        _vPTS = 0;
        _aClock.reset();
        _vClock.reset();
        _eClock.reset();
        
        _demuxerControl = std::make_shared<DemuxerControl>(_url);
        _demuxerObserver = std::make_shared<PlayerDemuxerObserver>();
        _demuxerObserver->setplayer(shared_from_this());
        _demuxerControl->setobserver(_demuxerObserver);
        _demuxerControl->start();
    }
    
    return true;;
}

bool Player::openAudio() {
    if (_demuxerControl->demuxer()->hasAudio()) {
        _audioControl = std::shared_ptr<AudioCodecControl>();
        _audioControl->setpacketQueue(_demuxerControl->aPacketQueue());
        return _audioControl->start(_demuxerControl->demuxer()->audioStream());
    }
    return false;
}

bool Player::openVideo() {
    if (_demuxerControl->demuxer()->hasVideo()) {
        _videoControl = std::shared_ptr<VideoCodecControl>();
        _videoControl->setpacketQueue(_demuxerControl->vPacketQueue());
        return _videoControl->start(_demuxerControl->demuxer()->videoStream());
    }
    return false;
}

bool Player::close() {
    if (_status == kPlayerClose) {
        LOG(DEBUG) << "Waiting audio/video decode stop.";
        
        Mutex r(&_renderMutex);
        while (_renderring) {
            pthread_cond_wait(&_renderCond, &_renderMutex);
        }
        
        _vPTS = 0;
        _aClock.reset();
        _vClock.reset();
        _eClock.reset();
        
        LOG(DEBUG) << "Audio/Video decode stopped.";
        
        if (_audioQueue) {
            _audioQueue->teardown();
        }
        
        _videoControl->stop();
        _audioControl->stop();
        _demuxerControl->stop();
        
        setStatus(kPlayerStoped);
    }
    
    return true;
}

void Player::quit() {
    setStatus(kPlayerQuit);
    pthread_join(_inputThread, nullptr);
}

bool Player::isQuit() {
    Mutex m(&_statusMutex);
    while (_status != kPlayerOpen &&
           _status != kPlayerPlaying &&
           _status != kPlayerClose &&
           _status != kPlayerQuit) {
        pthread_cond_wait(&_statusCond, &_statusMutex);
        LOG(DEBUG) << "thread loop wakeup: status:" << _status;
    }
    
    if (_status == kPlayerQuit) {
        return true;
    } else {
        return false;
    }
}

void *Player::inputThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->inputLoop();
    return nullptr;
}

void Player::inputLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerOpen:
                open();
                break;
            case kPlayerClose:
                close();
                break;
            default:
            {
                usleep(1000);
                break;
            }
        }
    }
}

void *Player::renderThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->renderLoop();
    return nullptr;
}

void Player::renderLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerPlaying:
            {
                Mutex m(&_renderMutex);
                _renderring = true;
                std::shared_ptr<Frame> frame = nullptr;// _vFrameQueue.pop();
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
                    _filter.processFrame(frame);
                    _vPTS = frame->pts;
                    _vClock.setClock(frame->pts);
                    LOG(TRACE) << "render delay2 " << delay;
                    if (delay < 0) {
                        LOG(WARNING) << "delay is negative " << delay;
                        delay = 0;
                    }
                    usleep(delay * 1000);
                }
                _renderring = false;
                pthread_cond_broadcast(&_renderCond);
                break;
            }
            default:
            {
                waitStatusChange();
                break;
            }
        }
    }
}

void Player::audioCodecCB(TT::AudioDesc &desc) {
    if (_audioQueue) {
        _audioQueue->setup(desc);
    }
}

std::shared_ptr<Frame> Player::audioQueueCB() {
    return nullptr;
//    if (_aFrameQueue.empty()) {
//        return nullptr;
//    } else {
//        std::shared_ptr<Frame> frame = _aFrameQueue.pop();
//        if (frame) {
//            _aClock.setClock(frame->pts);
//            LOG(TRACE) << "render audio frame " << frame->pts;
//        }
//
//        return frame;
//    }
}

void Player::setMasterSyncType(AVSyncClock clock) {
    if (clock == AV_SYNC_VIDEO_MASTER) {
        if (_demuxerControl->demuxer()->hasVideo())
            _clock = AV_SYNC_VIDEO_MASTER;
        else
            _clock = AV_SYNC_AUDIO_MASTER;
    } else if (clock == AV_SYNC_AUDIO_MASTER) {
        if (_demuxerControl->demuxer()->hasAudio())
            _clock = AV_SYNC_AUDIO_MASTER;
        else
            _clock = AV_SYNC_EXTERNAL_CLOCK;
    } else {
        _clock = AV_SYNC_EXTERNAL_CLOCK;
    }
}

double Player::getMasterClock() {
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

#pragma mark PlayerDemuxerObserver
void PlayerDemuxerObserver::opened() {
    std::shared_ptr<Player> player = _player.lock();
    if (player) {
        player->setMasterSyncType(AV_SYNC_AUDIO_MASTER);
        player->openVideo();
        player->openAudio();
        player->setStatus(kPlayerPlaying);
    }
}

void PlayerDemuxerObserver::closed() {
    
}

void PlayerDemuxerObserver::error() {
    
}

