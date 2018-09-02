//
//  TTStatusM.hpp
//  TTPlayerExample
//
//  Created by liang on 14/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTStatusM_hpp
#define TTStatusM_hpp

#include <map>
#include <vector>
#include <pthread.h>

#include "TTMutex.hpp"

namespace TT {
    class StatusM {
    public:
        StatusM(int status);
        ~StatusM();
        
        bool transferToStatus(int toStatus);
        
        typedef std::function<bool (int status)> StatusHandle;
        void registerStatusHandle(int status, StatusHandle handle);
        void addStatusTransfer(int fromStatus, int toStatus);
        bool handleForStatus();
        
        int curStatus;
    private:
        pthread_mutex_t _statusMutex;
        std::map<int, StatusHandle> _handleMap;
        std::map<int, std::shared_ptr<std::vector<int>>> _statusMap;
    };
}

#endif /* TTStatusM_hpp */
