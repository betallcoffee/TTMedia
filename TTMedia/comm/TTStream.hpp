//
//  TTStream.hpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTStream_hpp
#define TTStream_hpp

#include "TTComm.h"

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
        
        TT_PROPERTY_DECL(int, index);
        TT_PROPERTY_DECL(StreamType, type);
        TT_PROPERTY_DECL(uint32_t, timeScale);
        TT_PROPERTY_DECL(uint64_t, duration);
        TT_PROPERTY_DECL_READONLY(uint8_t *, extraData);
        TT_PROPERTY_DECL(size_t, extraSize);
        
        bool allocExtraData(size_t size);
        
        const AVStream *internalStream() const { return _internalStream; };
    private:
        AVStream *_internalStream;
    };
}

#endif /* TTStream_hpp */
