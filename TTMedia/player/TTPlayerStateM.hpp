//
//  TTPlayerStateM.hpp
//  TTPlayerExample
//
//  Created by liang on 23/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTPlayerStateM_hpp
#define TTPlayerStateM_hpp

#include <memory>

#include "TTDef.h"
#include "TTStateM.hpp"

namespace TT {
    class Player;
    
    enum class PlayerStateSignal {
        kNone,
        kPlay,
        kPlaying,
        kStop,
        kPause,
    };
    
    class PlayerStateM : public StateM {
    public:
        PlayerStateM(std::shared_ptr<Player> player);
        ~PlayerStateM();
        
    private:
        std::weak_ptr<Player> _player;
    };
    
    class PlayerState : public State {
    public:
        PlayerState(std::shared_ptr<Player> player);
        virtual ~PlayerState() {};
        TT_PROPERTY_DEF_READONLY_NOINIT(std::weak_ptr<Player>, player);
        
    protected:
        bool run() override;
    };
    
    class PlayerStateStopped : public PlayerState {
    public:
        PlayerStateStopped(std::shared_ptr<Player> player);
        ~PlayerStateStopped() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
    };
    
    class PlayerStateReady : public PlayerState {
    public:
        PlayerStateReady(std::shared_ptr<Player> player);
        ~PlayerStateReady() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
    };
    
    class PlayerStatePlaying : public PlayerState {
    public:
        PlayerStatePlaying(std::shared_ptr<Player> player);
        ~PlayerStatePlaying() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
    };
    
    class PlayerStatePaused : public PlayerState {
    public:
        PlayerStatePaused(std::shared_ptr<Player> player);
        ~PlayerStatePaused() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
    };
    
    class PlayerStateResuming : public PlayerState {
    public:
        PlayerStateResuming(std::shared_ptr<Player> player);
        ~PlayerStateResuming() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
    };
}

#endif /* TTPlayerStateM_hpp */
