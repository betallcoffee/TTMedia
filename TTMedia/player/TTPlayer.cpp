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

#include "TTPlayerStateM.hpp"

#include "TTHTTPIO.hpp"
#include "TTAudioCodec.hpp"

#include "TTDemuxer.hpp"
#include "TTDemuxerControl.hpp"
#include "TTCodecControl.hpp"
#include "TTRenderControl.hpp"

#include "TTFilter.hpp"

using namespace TT;

INITIALIZE_EASYLOGGINGPP

Player::Player()
: _clock(AV_SYNC_AUDIO_MASTER)
{
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
//     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
}

Player::~Player() {
    close();
}

bool Player::init() {
    _stateM = std::make_shared<PlayerStateM>(shared_from_this());
    return true;
}

bool Player::destroy() {
    return true;
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
    _url = url;
    _stateM->emit(static_cast<int>(PlayerStateSignal::kPlay));
}

void Player::stop() {
    _stateM->emit(static_cast<int>(PlayerStateSignal::kStop));
}

void Player::pause() {
    _stateM->emit(static_cast<int>(PlayerStateSignal::kPause));
}

void Player::resume() {
    _stateM->emit(static_cast<int>(PlayerStateSignal::kPlay));
}

bool Player::open() {
    _demuxerControl = std::make_shared<DemuxerControl>(_url);
    _demuxerObserver = std::make_shared<PlayerDemuxerObserver>();
    _demuxerObserver->setplayer(shared_from_this());
    _demuxerControl->setobserver(_demuxerObserver);
    _demuxerControl->start();
    
    return true;
}

bool Player::openAudio() {
    if (_demuxerControl->demuxer()->hasAudio()) {
        _audioControl = std::make_shared<AudioCodecControl>();
        _audioObserver = std::make_shared<AudioCodecObserver>();
        _audioObserver->setplayer(shared_from_this());
        _audioControl->setobserver(_audioObserver);
        _audioControl->setpacketQueue(_demuxerControl->aPacketQueue());
        return _audioControl->start(_demuxerControl->demuxer()->audioStream());
    }
    return false;
}

bool Player::openVideo() {
    if (_demuxerControl->demuxer()->hasVideo()) {
        _videoControl = std::make_shared<VideoCodecControl>();
        _videoObserver = std::make_shared<VideoCodecObserver>();
        _videoObserver->setplayer(shared_from_this());
        _videoControl->setobserver(_videoObserver);
        _videoControl->setpacketQueue(_demuxerControl->vPacketQueue());
        return _videoControl->start(_demuxerControl->demuxer()->videoStream());
    }
    return false;
}

bool Player::openVideoRender() {
    if (_videoControl->frameQueue()) {
        std::shared_ptr<Y420ToRGBFilter> filter = std::make_shared<Y420ToRGBFilter>();
        filter->addFilter(_bindFilter);
        _renderControl = std::make_shared<RenderControl>();
        _renderControl->setfilter(filter);
        _renderControl->setclock(_clock);
        _renderControl->setframeQueue(_videoControl->frameQueue());
        return _renderControl->start();
    }
    return false;
}

bool Player::close() {
    LOG(DEBUG) << "Waiting audio/video decode stop.";
    LOG(DEBUG) << "Audio/Video decode stopped.";
    
    if (_audioQueue) {
        _audioQueue->teardown();
    }
    
    _renderControl->stop();
    _videoControl->stop();
    _audioControl->stop();
    _demuxerControl->stop();
    
    return true;
}

void Player::openAudioPlay(TT::AudioDesc &desc) {
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
        if (frame && _renderContext) {
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
        player->openVideoRender();
    }
}

#pragma mark AudioCodecObserver
void AudioCodecObserver::audioDesc(TT::AudioDesc &desc) {
    std::shared_ptr<Player> player = _player.lock();
    if (player) {
        player->openAudioPlay(desc);
    }
}

