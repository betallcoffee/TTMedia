//
//  TTFrame.hpp
//  TTPlayerExample
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTFrame_hpp
#define TTFrame_hpp

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include "TTComm.h"

namespace TT {
    struct AudioDesc {
        int sampleRate;
        int channels;
        enum AVSampleFormat fmt;
        int nbSamples;
    };
    
    class Frame {
    public:
        Frame();
        Frame(AVFrame *avFrame, MediaType mediaType = kMediaTypeVideo, DataType dataType = kTextureTypeY420p);
        ~Frame();
        
        AVFrame *avFrame() { return _avFrame; }
        
        bool reallocData(size_t dataSize, int index);
        bool isKeyframe();
        
        enum {kNumOfPlanars = 3};
        
        MediaType mediaType;
        DataType dataType;
        
        int tag;
        
        uint8_t *data[kNumOfPlanars];
        size_t lineSize[kNumOfPlanars];
        size_t numOfPlanars;
        
        size_t width;
        size_t height;
        
        int64_t pts;
        int64_t pkt_pts;
        int64_t pkt_dts;
        
        enum AVSampleFormat sampleFormat;
        
    private:
        AVFrame *_avFrame;
        bool _isKeyFrame;
    };
}

#endif /* TTFrame_hpp */
