//
//  TTMessageLoop.cpp
//  TTPlayerExample
//
//  Created by liang on 22/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTMessageLoop.hpp"

using namespace TT;

MessageLoop::MessageLoop() :
_isRunning(true),
_msgMutex(PTHREAD_MUTEX_INITIALIZER),
_msgQueue(nullptr) {
    pthread_create(&_msgThread, nullptr, MessageLoop::messageThreadEntry, this);
}

MessageLoop::~MessageLoop() {
}

void MessageLoop::setMessageHandle(MessageLoop::MessageHandle handle) {
    Mutex m(&_msgMutex);
    _handle = handle;
}

void MessageLoop::stop() {
    Mutex m(&_msgMutex);
    _isRunning = false;
}

void MessageLoop::postMessage(int msgCode) {
    std::shared_ptr<Message> message = std::make_shared<Message>(msgCode);
    postMessage(message);
}
void MessageLoop::postMessage(std::shared_ptr<Message> message) {
    _msgQueue.push(message);
}

void *MessageLoop::messageThreadEntry(void *opaque) {
    pthread_setname_np("message loop thread");
    MessageLoop *self = static_cast<MessageLoop *>(opaque);
    self->messageThreadLoop();
    return nullptr;
}

void MessageLoop::messageThreadLoop() {
    while (_isRunning) {
        Mutex m(&_msgMutex);
        std::shared_ptr<Message> message = _msgQueue.pop();
        MessageHandle handle = message->handle();
        if (handle) {
            handle(message);
        } else if (_handle) {
            _handle(message);
        }
    }
}


