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

#include "TTdef.h"
#include "TTQueue.hpp"

namespace TT {
    class MessageLoop;
    class Message;
    class URL;
    class Demuxer;
    class Packet;
    
    typedef enum DemuxerMessage {
        kDemuxerOpen,
        kDemuxerRead,
        kDemuxerSeek,
        kDemuxerClose,
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
        
        bool seek(int64_t millisecond);
        
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<MessageLoop>, loop);
        
        TT_PROPERTY_DEF(std::weak_ptr<DemuxerObserver>, observer);
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<Demuxer>, demuxer);
        typedef Queue<std::shared_ptr<Packet>> PacketQueue;
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<PacketQueue>, vPacketQueue);
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<PacketQueue>, aPacketQueue);
        
    private:
        void initMessages();
        void handleMessage(std::shared_ptr<Message> message);
        void readPacket();
        
        std::shared_ptr<URL> _url;
    };
}

#endif /* TTDemuxerControl_hpp */
