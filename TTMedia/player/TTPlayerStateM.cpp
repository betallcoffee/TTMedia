//
//  TTPlayerStateM.cpp
//  TTMedia
//
//  Created by liang on 23/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTPlayerStateM.hpp"
#include "TTPlayer.hpp"

using namespace TT;

PlayerStateM::PlayerStateM(std::shared_ptr<Player> player)
: _player(player)
{
    std::shared_ptr<PlayerState> state = std::make_shared<PlayerStateStopped>(player);
    init(state);
}

PlayerStateM::~PlayerStateM() {
    
}

PlayerState::PlayerState(std::shared_ptr<Player> player)
: _player(player)
{
    
}

bool PlayerState::run() {
    return false;
}

PlayerStateStopped::PlayerStateStopped(std::shared_ptr<Player> player)
: PlayerState(player)
{
    
}

bool PlayerStateStopped::enter() {
    std::shared_ptr<Player> v = player().lock();
    if (v) {
        v->stop();
    }
    return true;
}

bool PlayerStateStopped::leave() {
    return true;
}

std::shared_ptr<State> PlayerStateStopped::slot(int signal) {
    PlayerStateSignal playerSignal = static_cast<PlayerStateSignal>(signal);
    if (playerSignal == PlayerStateSignal::kPlay) {
        return std::make_shared<PlayerStateReady>(player().lock());
    }
    
    return nullptr;
}

PlayerStateReady::PlayerStateReady(std::shared_ptr<Player> player)
: PlayerState(player)
{
    
}

bool PlayerStateReady::enter() {
    std::shared_ptr<Player> v = player().lock();
    if (v) {
        v->open();
    }
    return true;
}

bool PlayerStateReady::leave() {
    return true;
}

std::shared_ptr<State> PlayerStateReady::slot(int signal) {
    PlayerStateSignal playerSignal = static_cast<PlayerStateSignal>(signal);
    if (playerSignal == PlayerStateSignal::kPlaying) {
        return std::make_shared<PlayerStatePlaying>(player().lock());
    }
    
    return nullptr;
}


PlayerStatePlaying::PlayerStatePlaying(std::shared_ptr<Player> player)
: PlayerState(player)
{
    
}

bool PlayerStatePlaying::enter() {
    return true;
}

bool PlayerStatePlaying::leave() {
    std::shared_ptr<Player> v = player().lock();
    if (v) {
        v->stop();
    }
    return true;
}

std::shared_ptr<State> PlayerStatePlaying::slot(int signal) {
    PlayerStateSignal playerSignal = static_cast<PlayerStateSignal>(signal);
    if (playerSignal == PlayerStateSignal::kStop) {
        return std::make_shared<PlayerStateStopped>(player().lock());
    } else if (playerSignal == PlayerStateSignal::kPause) {
        return std::make_shared<PlayerStatePaused>(player().lock());
    }
    
    return nullptr;
}

PlayerStatePaused::PlayerStatePaused(std::shared_ptr<Player> player)
: PlayerState(player)
{
    
}

bool PlayerStatePaused::enter() {
    std::shared_ptr<Player> v = player().lock();
    if (v) {
        
    }
    return true;
}

bool PlayerStatePaused::leave() {
    return true;
}

std::shared_ptr<State> PlayerStatePaused::slot(int signal) {
    PlayerStateSignal playerSignal = static_cast<PlayerStateSignal>(signal);
    if (playerSignal == PlayerStateSignal::kStop) {
        return std::make_shared<PlayerStateStopped>(player().lock());
    } else if (playerSignal == PlayerStateSignal::kPlay) {
        return std::make_shared<PlayerStateResuming>(player().lock());
    }
    
    return nullptr;
}

PlayerStateResuming::PlayerStateResuming(std::shared_ptr<Player> player)
: PlayerState(player)
{
    
}

bool PlayerStateResuming::enter() {
    std::shared_ptr<Player> v = player().lock();
    if (v) {
        
    }
    return true;
}

bool PlayerStateResuming::leave() {
    return true;
}

std::shared_ptr<State> PlayerStateResuming::slot(int signal) {
    PlayerStateSignal playerSignal = static_cast<PlayerStateSignal>(signal);
    if (playerSignal == PlayerStateSignal::kStop) {
        return std::make_shared<PlayerStateStopped>(player().lock());
    } else if (playerSignal == PlayerStateSignal::kPlaying) {
        return std::make_shared<PlayerStatePlaying>(player().lock());
    }
    
    return nullptr;
}
