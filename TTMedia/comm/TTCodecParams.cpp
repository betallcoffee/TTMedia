//
//  TTCodecParams.cpp
//  TTPlayerExample
//
//  Created by liang on 6/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTCodecParams.hpp"

using namespace TT;

CodecParams::CodecParams() {
    
}

CodecParams::~CodecParams() {
    
}

std::shared_ptr<CodecParams> CodecParams::H264CodecParams(size_t width, size_t height) {
    std::shared_ptr<CodecParams> codecParams = std::make_shared<CodecParams>();
    codecParams->codecType = kMediaTypeVideo;
    codecParams->width = static_cast<int>(width);
    codecParams->height = static_cast<int>(height);
    codecParams->codecID = CodecID::kCodecIDH264;
    return codecParams;
}

