//
//  TTVideoCodec.hpp
//  TTPlayerExample
//
//  Created by liang on 9/9/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTVideoCodec_hpp
#define TTVideoCodec_hpp

#include <stdio.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include "TTCodecParams.hpp"
#include "TTCodec.hpp"

namespace TT {
    typedef enum {
        kVideoCodecNone,
        kVideoCodecDecode,
        kVideoCodecEncode
    } VideoCodecType;
    
    class VideoCodec : public Codec {
    public:
        VideoCodec(std::shared_ptr<CodecParams> codecParams, const AVStream *avStream, VideoCodecType type = kVideoCodecDecode);
        VideoCodec(const AVStream *avStream, VideoCodecType type = kVideoCodecDecode);
        ~VideoCodec();
        
        bool open() override;
        void close() override;
        void flush();
        std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet) override;
        void encode(std::shared_ptr<Frame> frame) override;
        
        typedef std::function<void (std::shared_ptr<Packet>)> EncodeFrameCallback;
        void setEncodeFrameCallback(EncodeFrameCallback cb) { _encodeFrameCallback = cb; }
        
    private:
        VideoCodecType _type;
        
        std::shared_ptr<CodecParams> _codecParams;
        const AVStream *_avStream;
        AVCodecContext *_avCodecContext;
        AVCodec *_avCodec;
        
        EncodeFrameCallback _encodeFrameCallback;
    };
}

#endif /* TTVideoCodec_hpp */
