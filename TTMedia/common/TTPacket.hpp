//
//  TTPacket.hpp
//  TTPlayerExample
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTPacket_hpp
#define TTPacket_hpp

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

#include "TTCommon.h"

namespace TT {
    class Packet {
    public:
        Packet();
        Packet(AVPacket *avpacket);
        ~Packet();
        
        bool allocData(size_t capacity);
        
        TT_PROPERTY_DECL_READONLY(uint8_t *, data);
        TT_PROPERTY_DECL_READONLY(size_t, capacity);
        TT_PROPERTY_DECL(size_t, size);
        
        PacketType type;
        int64_t dts;
        int64_t pts;
        int64_t pos;
        int64_t duration;
        
        int64_t audioSamplePerPacket;
        
        AVPacket *avpacket() { return _avpacket; }
        
    private:
        AVPacket *_avpacket;
    };
}

#endif /* TTPacket_hpp */
