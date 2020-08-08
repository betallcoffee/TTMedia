//
//  TTDemuxer_ios.m
//  TTPlayerExample
//
//  Created by liang on 19/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#import "TTDemuxer_ios.h"

using namespace TT;

Demuxer_ios::Demuxer_ios()
{
    
}

Demuxer_ios::~Demuxer_ios()
{
    
}

int Demuxer_ios::probe(TT_SP(URL) url)
{
    return kInvalidValue;
}

TT_SP(Demuxer) Demuxer_ios::createDemuxer(TT_SP(URL) url)
{
    TT_SP(Demuxer) demuxer = nullptr;
    
    
    if (demuxer == nullptr) {
        demuxer = Demuxer::createDemuxer(url);
    }
    
    return demuxer;
}

bool Demuxer_ios::open(TT_SP(URL) url)
{
    CALL_SEL_RETURN_1(Demuxer_dummy, open, url);
}

void Demuxer_ios::close()
{
    CALL_SEL(Demuxer_dummy, close);
}

TT_SP(Packet) Demuxer_ios::read()
{
    CALL_SEL_RETURN(Demuxer_dummy, read);
}

bool Demuxer_ios::seek(uint64_t pos)
{
    CALL_SEL_RETURN_1(Demuxer_dummy, seek, pos);
}

bool Demuxer_ios::isEOF()
{
    CALL_SEL_RETURN(Demuxer_dummy, isEOF);
}

bool Demuxer_ios::hasAudio()
{
    CALL_SEL_RETURN(Demuxer_dummy, hasAudio);
}

bool Demuxer_ios::hasVideo()
{
    CALL_SEL_RETURN(Demuxer_dummy, hasVideo);
}

TT_SP(Stream) Demuxer_ios::audioStream()
{
    CALL_SEL_RETURN(Demuxer_dummy, audioStream);
}

TT_SP(Stream) Demuxer_ios::videoStream()
{
    CALL_SEL_RETURN(Demuxer_dummy, videoStream);
}

TT_SP(CodecParams) Demuxer_ios::audioCodecParams()
{
    CALL_SEL_RETURN(Demuxer_dummy, audioCodecParams);
}

TT_SP(CodecParams) Demuxer_ios::videoCodecParams()
{
    CALL_SEL_RETURN(Demuxer_dummy, videoCodecParams);
}

TT_SP(AudioCodec) Demuxer_ios::audioCodec()
{
    CALL_SEL_RETURN(Demuxer_dummy, audioCodec);
}

TT_SP(VideoCodec) Demuxer_ios::videoCodec()
{
    CALL_SEL_RETURN(Demuxer_dummy, videoCodec);
}



