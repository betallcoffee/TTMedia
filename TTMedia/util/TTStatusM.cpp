//
//  TTStatusM.cpp
//  TTPlayerExample
//
//  Created by liang on 14/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTStatusM.hpp"

using namespace TT;

StatusM::StatusM(int status) : curStatus(status), _statusMutex(PTHREAD_MUTEX_INITIALIZER) {
    
}

StatusM::~StatusM() {
    
}

bool StatusM::transferToStatus(int toStatus) {
    Mutex m(&_statusMutex);
    std::shared_ptr<std::vector<int>> toStatusList = nullptr;
    std::map<int, std::shared_ptr<std::vector<int>>>::iterator it;
    it = _statusMap.find(curStatus);
    if (it == _statusMap.end()) {
        return false;
    } else {
        std::vector<int>::iterator toStatusIt;
        toStatusList = it->second;
        toStatusIt = std::find(toStatusList->begin(), toStatusList->end(), toStatus);
        if (toStatusIt == toStatusList->end()) {
            return false;
        }
        curStatus = toStatus;
        return true;
    }
}

void StatusM::registerStatusHandle(int status, StatusHandle handle) {
    Mutex m(&_statusMutex);
    _handleMap[status] = handle;
}

void StatusM::addStatusTransfer(int fromStatus, int toStatus) {
    Mutex m(&_statusMutex);
    std::shared_ptr<std::vector<int>> toStatusList = nullptr;
    std::map<int, std::shared_ptr<std::vector<int>>>::iterator it;
    it = _statusMap.find(fromStatus);
    if (it == _statusMap.end()) {
        toStatusList = std::make_shared<std::vector<int>>();
        _statusMap[fromStatus] = toStatusList;
    } else {
        toStatusList = it->second;
    }
    
    if (toStatusList) {
        toStatusList->push_back(toStatus);
    }
}

bool StatusM::handleForStatus() {
    StatusHandle handle;
    int status;
    {
        Mutex m(&_statusMutex);
        std::map<int, StatusHandle>::iterator handleIt;
        handleIt = _handleMap.find(curStatus);
        if (handleIt == _handleMap.end()) {
            return false;
        }
        handle = handleIt->second;
        status = curStatus;
    }
    
    if (handle) {
        return handle(status);
    } else {
        return false;
    }
}
