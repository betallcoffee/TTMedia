//
//  TTCodecParams.cpp
//  TTPlayerExample
//
//  Created by liang on 6/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTCodecParams.hpp"

using namespace TT;

CodecParams::CodecParams()
: width(0)
, height(0)
, profile(0)
, bitRate(0)
, channels(0)
, sampleRate(0)
{
    
}

CodecParams::~CodecParams() {
    
}

std::shared_ptr<CodecParams> CodecParams::AACCodecParams(uint32_t channels, uint32_t sampleRate) {
    std::shared_ptr<CodecParams> codecParams = std::make_shared<CodecParams>();
    codecParams->codecType = kMediaTypeAudio;
    codecParams->codecID = CodecID::kCodecIDAAC;
    codecParams->channels = channels;
    codecParams->sampleRate = sampleRate;
    codecParams->bitRate = 96 * 1024;
    codecParams->timeBase.num = 1;
    codecParams->timeBase.den = sampleRate;
    
    return codecParams;
}

std::shared_ptr<CodecParams> CodecParams::H264CodecParams(size_t width, size_t height) {
    std::shared_ptr<CodecParams> codecParams = std::make_shared<CodecParams>();
    codecParams->codecType = kMediaTypeVideo;
    codecParams->width = static_cast<int>(width);
    codecParams->height = static_cast<int>(height);
    codecParams->codecID = CodecID::kCodecIDH264;
    codecParams->profile = TT_PROFILE_H264_HIGH;
    codecParams->bitRate = 1000 * 1024;
    codecParams->gopSize = 5 * 30;
    codecParams->timeBase.num = 1;
    codecParams->timeBase.den = 30;
    
    return codecParams;
}




