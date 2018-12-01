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

#include "TTdef.h"
#include "TTQueue.hpp"

namespace TT {
    class MessageLoop;
    class Message;
    class URL;
    class Demuxer;
    class Packet;
    
    typedef enum DemuxerMessage {
        kOpen,
        kRead,
        kSeek,
        kClose,
    } DemuxerMessage;
    
    class DemuxerObserver {
    public:
        DemuxerObserver() {};
        virtual ~DemuxerObserver() {};
        
        virtual void opened() = 0;
        virtual void closed() = 0;
        virtual void error() = 0;
    };
    
    class DemuxerControl {
    public:
        DemuxerControl(std::shared_ptr<URL> url);
        ~DemuxerControl();
        
        bool start();
        bool stop();
        
//        const std::shared_ptr<MessageLoop> loop() const { return _loop; }
        
        bool seek(int64_t millisecond);
        
        TT_PROPERTY_DECL_READONLY(std::shared_ptr<Demuxer>, demuxer);
        TT_PROPERTY_DECL(Queue<std::shared_ptr<Packet>>, vPacketQueue);
        TT_PROPERTY_DECL(Queue<std::shared_ptr<Packet>>, aPacketQueue);
        TT_PROPERTY_DECL(std::weak_ptr<DemuxerObserver>, observer);
        
    private:
        void initMessages();
        void handleMessage(std::shared_ptr<Message> message);
        void readPacket();
        
        std::shared_ptr<MessageLoop> _loop;
        std::shared_ptr<URL> _url;
        bool _isDemuxing;
    };
}

#endif /* TTDemuxerControl_hpp */
