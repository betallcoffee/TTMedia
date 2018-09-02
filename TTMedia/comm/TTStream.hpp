//
//  TTStream.hpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTStream_hpp
#define TTStream_hpp

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
        
        const AVStream *internalStream() const { return _internalStream; };
    private:
        AVStream *_internalStream;
    };
}

#endif /* TTStream_hpp */
