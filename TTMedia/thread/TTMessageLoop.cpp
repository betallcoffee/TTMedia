//
//  TTMessageLoop.cpp
//  TTPlayerExample
//
//  Created by liang on 22/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTMessageLoop.hpp"

using namespace TT;

Message::Message(int code, MessageHandle handle)
: _code(code)
, _handle(handle)
{
    
}

MessageLoop::MessageLoop() :
_isRunning(true),
_msgMutex(PTHREAD_MUTEX_INITIALIZER),
_msgQueue(nullptr) {
    pthread_create(&_msgThread, nullptr, MessageLoop::messageThreadEntry, this);
}

MessageLoop::MessageLoop(const char *threadName)
: _isRunning(true)
, _threadName(threadName)
, _msgMutex(PTHREAD_MUTEX_INITIALIZER)
, _msgQueue(nullptr) {
    pthread_create(&_msgThread, nullptr, MessageLoop::messageThreadEntry, this);
}

MessageLoop::~MessageLoop() {
}

void MessageLoop::setMessageHandle(MessageLoop::MessageHandle handle) {
    _msgQueue.wakeup();
    Mutex m(&_msgMutex);
    _handle = handle;
}

bool MessageLoop::start() {
    _msgQueue.wakeup();
    Mutex m(&_msgMutex);
    _isRunning = true;
    return true;
}

void MessageLoop::stop() {
    _msgQueue.wakeup();
    Mutex m(&_msgMutex);
    _isRunning = false;
}

void MessageLoop::postMessage(int code) {
    std::shared_ptr<Message> message = std::make_shared<Message>(code);
    postMessage(message);
}

void MessageLoop::postMessage(std::shared_ptr<Message> message) {
    _msgQueue.push(message);
}

void MessageLoop::emitMessage(int code) {
    std::shared_ptr<Message> message = _handleMap[code];
    postMessage(message);
}

void MessageLoop::slotMessage(std::shared_ptr<Message> message) {
    _handleMap[message->code()] = message;
}

void *MessageLoop::messageThreadEntry(void *opaque) {
    MessageLoop *self = static_cast<MessageLoop *>(opaque);
    self->messageThreadLoop();
    return nullptr;
}

void MessageLoop::messageThreadLoop() {
    if (_threadName.c_str()) {
        pthread_setname_np(_threadName.c_str());
    } else {
        pthread_setname_np("message loop thread");
    }
    
    while (_isRunning) {
        Mutex m(&_msgMutex);
        std::shared_ptr<Message> message = _msgQueue.pop();
        if (message) {
            MessageHandle handle = message->handle();
            if (handle) {
                handle(message);
            } else if (_handle) {
                _handle(message);
            }
        }
    }
}


