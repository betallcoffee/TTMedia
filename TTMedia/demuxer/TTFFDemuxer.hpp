//
//  TTFFDemuxer.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTFFDemuxer_hpp
#define TTFFDemuxer_hpp

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

#include "TTCodecParams.hpp"
#include "TTDemuxer.hpp"

namespace TT {
    class FFDemuxer : public Demuxer {
    public:
        FFDemuxer();
        ~FFDemuxer() {};
        
        static int probe(std::shared_ptr<URL> url);
        
        bool open(std::shared_ptr<URL> url) override;
        void close() override;
        
        std::shared_ptr<Packet> read() override;
        
        bool seek(uint64_t pos) override;
        bool isEOF() override { return _isEOF; };
        
        bool hasAudio() override { return _audioStream == nullptr ? false : true; }
        bool hasVideo() override { return _videoStream == nullptr ? false : true; }
        
        std::shared_ptr<AudioCodec> audioCodec() override;
        std::shared_ptr<VideoCodec> videoCodec() override;
        
    private:
        std::shared_ptr<URL> _url;
        
        pthread_mutex_t _mutex;
        bool _isEOF = false;
        
        AVFormatContext *_formatContext;
        AVDictionary *_option;
    };
}

#endif /* TTFFDemuxer_hpp */
