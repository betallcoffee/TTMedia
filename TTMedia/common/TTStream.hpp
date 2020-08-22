//
//  TTStream.hpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTStream_hpp
#define TTStream_hpp

#include "TTCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    #include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

namespace TT {
    class Stream {
    public:
        Stream();
        Stream(AVStream *stream);
        virtual ~Stream();
        
        TT_PROPERTY_DEF(int, index, 0);
        TT_PROPERTY_DEF(StreamType, type, kStreamTypeNone);
        TT_PROPERTY_DEF(CodecID, codecID, kCodecIDNone);
        TT_PROPERTY_DEF(TTRational, timeScale, {});
        TT_PROPERTY_DEF(uint64_t, duration, 0);
        TT_PROPERTY_DEF_READONLY(uint8_t *, extraData, nullptr);
        TT_PROPERTY_DEF(size_t, extraSize, 0);
        
        bool allocExtraData(size_t size);
        
        const AVStream *internalStream() const { return _internalStream; };
    private:
        AVStream *_internalStream;
    };
}

#endif /* TTStream_hpp */
