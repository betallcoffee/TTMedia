//
//  TTStateM.cpp
//  TTPlayerExample
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

void StateM::init(std::shared_ptr<State> state) {
    Mutex m(&_stateMutex);
    _state = state;
}

void StateM::emit(int signal) {
    Mutex m(&_stateMutex);
    std::shared_ptr<State> state = _state->slot(signal);
    if (state) {
        _state->leave();
        _state = state;
        _state->enter();
    }
}

int StateM::run() {
    Mutex m(&_stateMutex);
    int signal = _state->run();
    std::shared_ptr<State> state = _state->slot(signal);
    if (state) {
        _state->leave();
        _state = state;
        _state->enter();
    }
    
    return signal;
}
