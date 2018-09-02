//
//  TTMessageLoop.hpp
//  TTPlayerExample
//
//  Created by liang on 22/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTMessageLoop_hpp
#define TTMessageLoop_hpp

#include <pthread.h>
#include "TTQueue.hpp"

namespace TT {
    
    class Message {
    public:
        Message(int code) { _code = code; }
        ~Message() {}
        
        int code() { return _code; }
        
        typedef std::function<void(std::shared_ptr<Message> msg)> MessageHandle;
        void setMessageHandle(MessageHandle handle) { _handle = handle; }
        MessageHandle handle() { return _handle; }
        
    private:
        int _code;
        MessageHandle _handle;
    };
    
    class MessageLoop {
    public:
        MessageLoop();
        ~MessageLoop();
        
        typedef std::function<void(std::shared_ptr<Message> msg)> MessageHandle;
        void setMessageHandle(MessageHandle handle);
        
        void stop();
        
        void postMessage(int msgCode);
        void postMessage(std::shared_ptr<Message> message);
        
    private:
        static void *messageThreadEntry(void *opaque);
        void messageThreadLoop();
        
        Queue<std::shared_ptr<Message>> _msgQueue;
        MessageHandle _handle;
        
        bool _isRunning;
        pthread_t _msgThread;
        pthread_mutex_t _msgMutex;
    };
}

#endif /* TTMessageLoop_hpp */
