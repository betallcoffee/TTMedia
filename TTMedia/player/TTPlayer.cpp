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
#include "TTRenderControl.hpp"

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
 _clock(AV_SYNC_AUDIO_MASTER) {
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
     pthread_create(&_inputThread, nullptr, Player::inputThreadEntry, this);
}

Player::~Player() {
    close();
}

void Player::bindRenderContext(std::shared_ptr<RenderContext> context) {
    _renderContext = context;
}

void Player::bindAudioQueue(std::shared_ptr<AudioQueue> audioQueue) {
    _audioQueue = audioQueue;
    if (_audioQueue) {
        _audioQueue->setQueueCallback(std::bind(&Player::audioQueueCB, this));
    }
}

void Player::bindFilter(std::shared_ptr<Filter> filter) {
    _bindFilter = filter;
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

bool Player::openRender() {
    if (_videoControl->frameQueue()) {
        std::shared_ptr<Y420ToRGBFilter> filter = std::make_shared<Y420ToRGBFilter>();
        filter->addFilter(_bindFilter);
        _renderControl = std::shared_ptr<RenderControl>();
        _renderControl->setfilter(filter);
        _renderControl->setclock(_clock);
        _renderControl->setframeQueue(_videoControl->frameQueue());
        return _renderControl->start();
    }
    return false;
}

bool Player::close() {
    if (_status == kPlayerClose) {
        LOG(DEBUG) << "Waiting audio/video decode stop.";
        
        LOG(DEBUG) << "Audio/Video decode stopped.";
        
        if (_audioQueue) {
            _audioQueue->teardown();
        }
        
        _renderControl->stop();
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

void Player::audioCodecCB(TT::AudioDesc &desc) {
    if (_audioQueue) {
        _audioQueue->setup(desc);
    }
}

std::shared_ptr<Frame> Player::audioQueueCB() {
    if (_audioControl == nullptr) {
        return nullptr;
    }
    
    std::shared_ptr<CodecControl::FrameQueue> frameQueue = _audioControl->frameQueue();
    if (frameQueue == nullptr) {
        return nullptr;
    }
    
    if (frameQueue->empty()) {
        return nullptr;
    } else {
        std::shared_ptr<Frame> frame = frameQueue->pop();
        if (frame) {
            _renderControl->updateAudioClock(frame->pts);
            LOG(TRACE) << "render audio frame " << frame->pts;
        }

        return frame;
    }
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

#pragma mark VideoCodecObserver
void VideoCodecObserver::opened() {
    std::shared_ptr<Player> player = _player.lock();
    if (player) {
    }
}

void VideoCodecObserver::closed() {
    
}

void VideoCodecObserver::error() {
    
}

