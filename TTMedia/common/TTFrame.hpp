//
//  TTFrame.hpp
//  TTMedia
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

#include "TTDef.h"
#include "TTCommon.h"

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
        ~Frame();
        
        Frame(AVFrame *avFrame, MediaType mediaType = kMediaTypeVideo, DataType dataType = kTextureTypeY420p);
        Frame(const uint8_t *srcData, size_t srcDataSize, MediaType mediaType, DataType dataType);
        
        AVFrame *avFrame() { return _avFrame; }
        
        bool reallocData(size_t dataSize, int index);
        bool isKeyframe();
        
        enum {kNumOfPlanars = 3};
        
        int tag = 0;
        
        uint8_t *data[kNumOfPlanars];
        size_t lineSize[kNumOfPlanars];
        size_t numOfPlanars = kNumOfPlanars;
        
        int64_t pts = AV_NOPTS_VALUE;
        int64_t pkt_pts = AV_NOPTS_VALUE;
        int64_t pkt_dts = AV_NOPTS_VALUE;
        
        enum AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE;
        
        TT_PROPERTY_DEF(MediaType, mediaType, kMediaTypeNone);
        TT_PROPERTY_DEF(DataType, dataType, kTextureTypeRGB);
        
        TT_PROPERTY_DEF(size_t, width, 0);
        TT_PROPERTY_DEF(size_t, height, 0);
        
    private:
        AVFrame *_avFrame = nullptr;
        bool _isKeyFrame = false;
    };
}

#endif /* TTFrame_hpp */
