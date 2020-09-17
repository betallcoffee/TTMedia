//
//  TTFFMuxer.hpp
//  TTMedia
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

#include "TTMuxer.hpp"
#include "TTVideoCodec.hpp"
#include "TTAudioCodec.hpp"


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
        bool createAudioStream(std::shared_ptr<CodecParams> codecParams);
        bool createVideoStream(std::shared_ptr<CodecParams> codecParams);
        
        void encodeAudioFrameCallback(std::shared_ptr<Packet> packet);
        void encodeVideoFrameCallback(std::shared_ptr<Packet> packet);
        
        std::shared_ptr<URL> _url = nullptr;
        
        pthread_mutex_t _mutex;
        
        AVFormatContext *_formatContext = nullptr;
        AVDictionary *_option = nullptr;
        
        AVStream *_audioStream = nullptr;
        std::shared_ptr<CodecParams> _audioCodecParams = nullptr;
        std::shared_ptr<AudioCodec> _audioCodec = nullptr;
        uint64_t _audioPts = 0;
        
        AVStream *_videoStream = nullptr;
        std::shared_ptr<CodecParams> _videoCodecParams = nullptr;
        std::shared_ptr<VideoCodec> _videoCodec = nullptr;
        uint64_t _videoPts = 0;
    };
}

#endif /* TTFFMuxer_hpp */
