//
//  TTMessageLoop.hpp
//  TTMedia
//
//  Created by liang on 22/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTMessageLoop_hpp
#define TTMessageLoop_hpp

#include <string>
#include <pthread.h>
#include <map>

#include "TTDef.h"

#include "TTQueue.hpp"

namespace TT {
    
    class Message {
    public:
        typedef std::function<void(std::shared_ptr<Message> msg)> MessageHandle;
        
        Message(int code, MessageHandle handle = nullptr);
        ~Message() {}
        
        int code() { return _code; }
        
        void setMessageHandle(MessageHandle handle) { _handle = handle; }
        MessageHandle handle() { return _handle; }
        
    private:
        int _code;
        MessageHandle _handle;
    };
    
    class MessageLoop {
    public:
        MessageLoop();
        MessageLoop(const char *threadName);
        ~MessageLoop();
        
        typedef std::function<void(std::shared_ptr<Message> msg)> MessageHandle;
        void setMessageHandle(MessageHandle handle);
        
        bool start();
        void stop();
        
        void postMessage(int code);
        void postMessage(std::shared_ptr<Message> message);
        
        void emitMessage(int code);
        void slotMessage(std::shared_ptr<Message> message);
        
    private:
        static void *messageThreadEntry(void *opaque);
        void messageThreadLoop();
        
        Queue<std::shared_ptr<Message>> _msgQueue;
        MessageHandle _handle;
        std::map<int, std::shared_ptr<Message>> _handleMap;
        
        bool _isRunning;
        std::string _threadName;
        pthread_t _msgThread;
        pthread_mutex_t _msgMutex;
    };
}

#endif /* TTMessageLoop_hpp */
