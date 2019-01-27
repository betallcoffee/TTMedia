//
//  TTFFMuxer.hpp
//  TTPlayerExample
//
//  Created by liang on 19/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTFFMuxer_hpp
#define TTFFMuxer_hpp

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

#include "TTVideoCodec.hpp"
#include "TTMuxer.hpp"


namespace TT {
    class FFMuxer : public Muxer {
    public:
        FFMuxer();
        ~FFMuxer();
        
        bool open(std::shared_ptr<URL> url,
                  std::shared_ptr<CodecParams> audioCodecParams,
                  std::shared_ptr<CodecParams> videoCodecParams) override;
        void close() override;
        void flush() override;
        bool write(std::shared_ptr<Packet> packet) override;
        bool write(std::shared_ptr<Frame> frame) override;
        
    private:
        void encodeFrameCallback(std::shared_ptr<Packet> packet);
        
        std::shared_ptr<URL> _url = nullptr;
        
        pthread_mutex_t _mutex;
        
        AVFormatContext *_formatContext = nullptr;
        AVDictionary *_option = nullptr;
        
        AVStream *_audioStream = nullptr;
        std::shared_ptr<CodecParams> _audioCodecParams = nullptr;
        AVStream *_videoStream = nullptr;
        std::shared_ptr<CodecParams> _videoCodecParams = nullptr;
        uint64_t _videoPts = 0;
        
        std::shared_ptr<VideoCodec> _videoCodec;
    };
}

#endif /* TTFFMuxer_hpp */
