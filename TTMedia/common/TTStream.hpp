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
        
        TT_PROPERTY_DEF(int, index);
        TT_PROPERTY_DEF(StreamType, type);
        TT_PROPERTY_DEF(CodecID, codecID);
        TT_PROPERTY_DEF(TTRational, timeScale);
        TT_PROPERTY_DEF(uint64_t, duration);
        TT_PROPERTY_DEF_READONLY(uint8_t *, extraData);
        TT_PROPERTY_DEF(size_t, extraSize);
        
        bool allocExtraData(size_t size);
        
        const AVStream *internalStream() const { return _internalStream; };
    private:
        AVStream *_internalStream;
    };
}

#endif /* TTStream_hpp */
