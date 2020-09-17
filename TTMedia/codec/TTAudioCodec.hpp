//
//  TTAudioCodec.hpp
//  TTMedia
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTAudioCodec_hpp
#define TTAudioCodec_hpp

#include <stdio.h>
#include <string>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
};
#endif

#include "TTDef.h"
#include "TTCodecParams.hpp"
#include "TTCodec.hpp"

namespace TT {
    
    class AudioDesc;
    
    typedef void (AudioCodecCB)(AudioDesc &desc);
    
    class AudioCodec : public Codec {
    public:
        AudioCodec(const AVStream *avStream, CodecType type = CodecType::kDecode);
        AudioCodec(std::shared_ptr<CodecParams> codecParams,
                   const AVStream *avStream,
                   CodecType type = CodecType::kDecode);
        ~AudioCodec();
        
        bool open() override;
        void close() override;
        void flush() override;
        std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet) override;
        bool encode(std::shared_ptr<Frame> frame) override;
        
        void setCodecCallback(std::function<AudioCodecCB> cb) { _cb = cb; }
        
        typedef std::function<void (std::shared_ptr<Packet>)> EncodeFrameCallback;
        TT_PROPERTY_DEF(EncodeFrameCallback, encodeFrameCallback, nullptr);
        
    private:
        void createSwrContext();
        
    private:
        CodecType _type;
        std::shared_ptr<CodecParams> _codecParams;
        
        const AVStream *_avStream;
        AVCodecContext *_avCodecContext;
        AVCodec *_avCodec;
        AVFrame *_avFrame;
        SwrContext *_swrContext;
        
        int _sampleRate;
        int64_t _channelLayout;
        enum AVSampleFormat _fmt;
        int _nbSamples;
        
        std::function<AudioCodecCB> _cb;
    };
}

#endif /* TTAudioCodec_hpp */
