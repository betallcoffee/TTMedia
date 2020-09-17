//
//  TTCond.hpp
//  TTMedia
//
//  Created by liang on 15/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTCond_hpp
#define TTCond_hpp

#include <assert.h>
#include <pthread.h>
#include <functional>

#include "TTMutex.hpp"

namespace TT {
    class Cond {
    public:
        explicit Cond(pthread_mutex_t *mutex) : _cond(PTHREAD_COND_INITIALIZER), _mutex(mutex) {
            assert(_mutex);
        }
        ~Cond() {
        }
        
        void notify(std::function<void ()> handle) {
            assert(_mutex);
            Mutex m(_mutex);
            handle();
            pthread_cond_broadcast(&_cond);
        }
        void wait(std::function<bool()> condition, std::function<void ()> handle) {
            assert(_mutex);
            assert(condition);
            assert(handle);
            
            Mutex m(_mutex);
            while (!condition()) {
                pthread_cond_wait(&_cond, _mutex);
            }
            
            handle();
        }
        
    private:
        Cond(const Cond &c);
        const Cond & operator=(const Cond &c);
        
        pthread_cond_t _cond;
        pthread_mutex_t *_mutex;
    };
}

#endif /* TTCond_hpp */
