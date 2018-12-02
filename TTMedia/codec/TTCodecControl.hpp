//
//  TTCodecControl.hpp
//  TTPlayerExample
//
//  Created by liang on 2/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTCodecControl_hpp
#define TTCodecControl_hpp

#include <memory>

#include "TTdef.h"
#include "TTQueue.hpp"

namespace TT {
    class MessageLoop;
    class Message;
    
    class Codec;
    class Packet;
    class Frame;
    class Stream;
    
    typedef enum CodecMessage {
        kCodecOpen,
        kCodecDecode,
        kCodecClose,
    } CodecMessage;
    
    class CodecControl {
    public:
        CodecControl();
        virtual ~CodecControl();
        
        virtual bool start(std::shared_ptr<Stream> stream);
        virtual bool stop();
        
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<MessageLoop>, loop);
        
        TT_PROPERTY_DEF(std::shared_ptr<Codec>, codec);
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<Stream>, stream);
        
        typedef Queue<std::shared_ptr<Packet>> PacketQueue;
        TT_PROPERTY_DEF(std::shared_ptr<PacketQueue>, packetQueue);
        typedef Queue<std::shared_ptr<Frame>> FrameQueue;
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<FrameQueue>, frameQueue);
        
    private:
        void initMessages();
        void handleMessage(std::shared_ptr<Message> message);
        
        virtual void decodePacket() {};
    };
    
    class VideoCodecControl : public CodecControl {
    public:
        VideoCodecControl();
        ~VideoCodecControl();
        
        bool start(std::shared_ptr<Stream> stream) override;
    private:
        void decodePacket() override;
    };
    
    class AudioCodecControl : public CodecControl {
    public:
        AudioCodecControl();
        ~AudioCodecControl();
        
        bool start(std::shared_ptr<Stream> stream) override;        
    private:
        void decodePacket() override;
    };
}

#endif /* TTCodecControl_hpp */
