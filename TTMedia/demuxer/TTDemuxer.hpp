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

#include "TTdef.h"
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
    static TT_SP(Demuxer) createDemuxer(std::shared_ptr<URL> url);
    
    virtual bool open(std::shared_ptr<URL> url) = 0;
    virtual void close() = 0;
    
    virtual std::shared_ptr<Packet> read() = 0;
    
    virtual bool seek(uint64_t pos) = 0;
    virtual bool isEOF() = 0;
    
    virtual bool hasAudio() {
        return _audioStream == nullptr ? false : true;
    }
    
    virtual bool hasVideo() {
        return _videoStream == nullptr ? false : true;
    }
    
    virtual TT_SP(Stream) audioStream() { return _audioStream; }
    virtual TT_SP(Stream) videoStream() { return _videoStream; }
    
    virtual TT_SP(CodecParams) audioCodecParams() { return _audioCodecParams; }
    virtual TT_SP(CodecParams) videoCodecParams() { return _videoCodecParams; }
    
    virtual TT_SP(AudioCodec) audioCodec() { return _audioCodec; }
    virtual TT_SP(VideoCodec) videoCodec() { return _videoCodec; }
    
protected:
    std::shared_ptr<Stream> _audioStream = nullptr;
    std::shared_ptr<Stream> _videoStream = nullptr;
    
    std::shared_ptr<CodecParams> _audioCodecParams = nullptr;
    std::shared_ptr<CodecParams> _videoCodecParams = nullptr;
    
    std::shared_ptr<AudioCodec> _audioCodec = nullptr;
    std::shared_ptr<VideoCodec> _videoCodec = nullptr;
    
    bool _isOpened = false;
};

}

#endif /* TTDemuxer_hpp */
