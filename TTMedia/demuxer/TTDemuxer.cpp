//
//  TTDemuxer.cpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTDemuxer.hpp"
#include "TTFFDemuxer.hpp"
#include "TTMP4Demuxer.hpp"

using namespace TT;

Demuxer::Demuxer()
{
    
}

Demuxer::~Demuxer() {
    
}

int Demuxer::probe(TT_SP(URL) url) {
    return 0;
}

std::shared_ptr<Demuxer> Demuxer::createDemuxer(std::shared_ptr<URL> url) {
    int score = 0;
    score = MP4Demuxer::probe(url);
    if (score > 0) {
        return std::make_shared<MP4Demuxer>();
    }
    score = FFDemuxer::probe(url);
    if (score > 0) {
        return std::make_shared<FFDemuxer>();
    }
    return nullptr;
}
