//
//  TTStateM.hpp
//  TTPlayerExample
//
//  Created by liang on 14/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTStateM_hpp
#define TTStateM_hpp

#include <map>
#include <vector>
#include <pthread.h>

#include "TTdef.h"
#include "TTMutex.hpp"

namespace TT {
    
    class StateM;
    
    class State {
    public:
        State() {};
        virtual ~State() {};
        
        friend StateM;
    protected:
        virtual bool enter() = 0;
        virtual bool leave() = 0;
        virtual std::shared_ptr<State> slot(int signal) = 0;
        
        virtual bool run() = 0;
    };
    
    class StateM {
    public:
        StateM();
        virtual ~StateM();
        
        bool init(std::shared_ptr<State> state);
        void emit(int signal);
        
        bool run();
        
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<State>, state);
    private:
        pthread_mutex_t _stateMutex;
    };
}

#endif /* TTStateM_hpp */
