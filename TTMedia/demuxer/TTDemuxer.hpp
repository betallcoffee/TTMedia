//
//  TTDemuxer.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTDemuxer_hpp
#define TTDemuxer_hpp

#include <stdio.h>
#include <unistd.h>

#include "TTURL.hpp"

#include "TTPacket.hpp"
#include "TTCodecParams.hpp"
#include "TTStream.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

namespace TT {
    class Demuxer {
    public:
        Demuxer();
        ~Demuxer();
        
        static int probe(std::shared_ptr<URL> url);
        static std::shared_ptr<Demuxer> createDemuxer(std::shared_ptr<URL> url);
        
        virtual bool open(std::shared_ptr<URL> url) = 0;
        virtual void close() = 0;
        
        virtual std::shared_ptr<Packet> read() = 0;
        
        virtual bool seek(uint64_t pos) = 0;
        virtual bool isEOF() = 0;
        
        virtual bool hasAudio() = 0;
        virtual bool hasVideo() = 0;
        
        std::shared_ptr<Stream> audioStream();
        std::shared_ptr<Stream> videoStream();
        
        std::shared_ptr<CodecParams> audioCodecParams();
        std::shared_ptr<CodecParams> videoCodecParams();
        
        virtual std::shared_ptr<AudioCodec> audioCodec() = 0;
        virtual std::shared_ptr<VideoCodec> videoCodec() = 0;
        
    protected:
        std::shared_ptr<Stream> _audioStream;
        std::shared_ptr<Stream> _videoStream;
        
        std::shared_ptr<CodecParams> _audioCodecParams;
        std::shared_ptr<CodecParams> _videoCodecParams;
        
        std::shared_ptr<AudioCodec> _audioCodec;
        std::shared_ptr<VideoCodec> _videoCodec;
        
        bool _isOpened;
    };
}

#endif /* TTDemuxer_hpp */
