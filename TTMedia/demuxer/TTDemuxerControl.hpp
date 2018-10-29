//
//  TTDemuxerControl.hpp
//  TTPlayerExample
//
//  Created by liang on 28/10/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTDemuxerControl_hpp
#define TTDemuxerControl_hpp

#include <memory>
#include <pthread.h>

namespace TT {
    class Demuxer;
    class MessageLoop;
    
    typedef enum DemuxerMessage {
        kOpen,
        kRead,
        kSeek,
        kClose,
    } eDemuxerMessage;
    
    class DemuxerControl {
    public:
        DemuxerControl();
        ~DemuxerControl();
        
        bool start();
        bool stop();
        
    private:
        std::shared_ptr<Demuxer> _demuxer;
        std::shared_ptr<MessageLoop> _loop;
    };
}

#endif /* TTDemuxerControl_hpp */
