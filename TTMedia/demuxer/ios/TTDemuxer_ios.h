//
//  TTDemuxer_ios.h
//  TTMedia
//
//  Created by liang on 19/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTDemuxer_ios_h
#define TTDemuxer_ios_h

#include <Foundation/Foundation.h>

#include "TTDef.h"
#include "TTDefForObjc.h"
#include "TTDemuxer.hpp"

@protocol TTDemuxerDelegate <NSObject>

@required
- (TT_SP(TT::Demuxer))demuxer;

- (BOOL)open:(TT_SP(TT::URL))url;
- (void)close;

- (TT_SP(TT::Packet))read;

- (BOOL)seek:(uint64_t)pos;
- (BOOL)isEOF;

- (BOOL)hasAudio;
- (BOOL)hasVideo;

- (TT_SP(TT::Stream))audioStream;
- (TT_SP(TT::Stream))videoStream;

- (TT_SP(TT::CodecParams))audioCodecParams;
- (TT_SP(TT::CodecParams))videoCodecParams;

- (TT_SP(TT::AudioCodec))audioCodec;
- (TT_SP(TT::VideoCodec))videoCodec;

@end

@interface TTDemuxer : NSObject <TTDemuxerDelegate>

@end


namespace TT {

class Demuxer_dummy : public Demuxer {
public:
    Demuxer_dummy() {};
    ~Demuxer_dummy() {};
    
    bool open(TT_SP(URL) url) override { return false; }
    void close() override {}
    
    TT_SP(Packet) read() override { return nullptr; }
    
    bool seek(uint64_t pos) override { return false; }
    bool isEOF() override { return false; }
};

class Demuxer_ios : public Demuxer_dummy {
public:
    Demuxer_ios();
    ~Demuxer_ios();
    
    static int probe(TT_SP(URL) url);
    static TT_SP(Demuxer) createDemuxer(TT_SP(URL) url);
    
    bool open(TT_SP(URL) url) override;
    void close() override;
    
    TT_SP(Packet) read() override;
    
    bool seek(uint64_t pos) override;
    bool isEOF() override;
    
    bool hasAudio() override;
    bool hasVideo() override;
    
    TT_SP(Stream) audioStream() override;
    TT_SP(Stream) videoStream() override;
    
    TT_SP(CodecParams) audioCodecParams() override;
    TT_SP(CodecParams) videoCodecParams() override;
    
    TT_SP(AudioCodec) audioCodec() override;
    TT_SP(VideoCodec) videoCodec() override;
    
    OBJC_DELEGATE(TTDemuxerDelegate)
};

}


#endif /* TTDemuxer_ios_h */
