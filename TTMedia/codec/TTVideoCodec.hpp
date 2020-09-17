//
//  TTVideoCodec.hpp
//  TTMedia
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
    class VideoCodec : public Codec {
    public:
        VideoCodec(const AVStream *avStream, CodecType type = CodecType::kDecode);
        VideoCodec(std::shared_ptr<CodecParams> codecParams,
                   const AVStream *avStream,
                   CodecType type = CodecType::kDecode);
        ~VideoCodec();
        
        bool open() override;
        void close() override;
        void flush() override;
        std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet) override;
        bool encode(std::shared_ptr<Frame> frame) override;
        
        typedef std::function<void (std::shared_ptr<Packet>)> EncodeFrameCallback;
        void setEncodeFrameCallback(EncodeFrameCallback cb) { _encodeFrameCallback = cb; }
        
    private:
        CodecType _type;
        
        std::shared_ptr<CodecParams> _codecParams;
        const AVStream *_avStream;
        AVCodecContext *_avCodecContext;
        AVCodec *_avCodec;
        
        EncodeFrameCallback _encodeFrameCallback;
    };
}

#endif /* TTVideoCodec_hpp */
