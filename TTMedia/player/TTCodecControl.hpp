//
//  TTCodecControl.hpp
//  TTMedia
//
//  Created by liang on 2/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTCodecControl_hpp
#define TTCodecControl_hpp

#include <memory>

#include "TTDef.h"
#include "TTQueue.hpp"
#include "TTFrame.hpp"

namespace TT {
    class MessageLoop;
    class Message;
    
    class Codec;
    class Packet;
    class Stream;
    
    typedef enum CodecMessage {
        kCodecOpen,
        kCodecDecode,
        kCodecClose,
    } CodecMessage;
    
    class CodecObserver {
    public:
        CodecObserver() {};
        virtual ~CodecObserver() {};
        
        virtual void opened() = 0;
        virtual void closed() = 0;
        virtual void error() = 0;
        
        virtual void audioDesc(AudioDesc &desc) = 0;
    };
    
    class CodecControl {
    public:
        CodecControl(const char *name);
        virtual ~CodecControl();
        
        virtual bool start(std::shared_ptr<Stream> stream);
        virtual bool stop();
        
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<MessageLoop>, loop, nullptr);
        TT_PROPERTY_DEF_NOINIT(std::weak_ptr<CodecObserver>, observer);
        
        TT_PROPERTY_DEF(std::shared_ptr<Codec>, codec, nullptr);
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<Stream>, stream, nullptr);
        
        typedef Queue<std::shared_ptr<Packet>> PacketQueue;
        TT_PROPERTY_DEF(std::shared_ptr<PacketQueue>, packetQueue, nullptr);
        typedef Queue<std::shared_ptr<Frame>> FrameQueue;
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<FrameQueue>, frameQueue, nullptr);
        
    private:
        virtual void initMessages();
        void handleMessage(std::shared_ptr<Message> message);
        
    protected:
        virtual bool open(std::shared_ptr<Stream> stream);
        virtual void decodePacket() {};
    };
    
    class VideoCodecControl : public CodecControl {
    public:
        VideoCodecControl();
        ~VideoCodecControl();
        
    private:
        bool open(std::shared_ptr<Stream> stream) override;
        void decodePacket() override;
    };
    
    class AudioCodecControl : public CodecControl {
    public:
        AudioCodecControl();
        ~AudioCodecControl();
        
        TT_PROPERTY_DEF_READONLY(AudioDesc, desc, {0});
              
    private:
        bool open(std::shared_ptr<Stream> stream) override;
        void decodePacket() override;
        
        void audioCodecCB(AudioDesc &desc);
    };
}

#endif /* TTCodecControl_hpp */
