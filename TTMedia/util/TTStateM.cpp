//
//  TTStateM.cpp
//  TTMedia
//
//  Created by liang on 14/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTStateM.hpp"

using namespace TT;

StateM::StateM()
: _stateMutex(PTHREAD_MUTEX_INITIALIZER)
{
    
}

StateM::~StateM() {
    
}

bool StateM::init(std::shared_ptr<State> state) {
    Mutex m(&_stateMutex);
    _state = state;
    
    return true;
}

void StateM::emit(int signal) {
    bool enterNewState = false;
    {
        Mutex m(&_stateMutex);
        std::shared_ptr<State> state = _state->slot(signal);
        if (state) {
            _state->leave();
            _state = state;
            enterNewState = true;
        }
    }
    
    if (enterNewState) {
        _state->enter();
    }
}

bool StateM::run() {
    std::shared_ptr<State> state = nullptr;
    {
        Mutex m(&_stateMutex);
        state = _state;
    }
    return state->run();
}
