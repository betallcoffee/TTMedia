//
//  TTFFWriter.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTFFWriter.hpp"

using namespace TT;

FFWriter::FFWriter() {
    
}

FFWriter::~FFWriter() {
    
}

bool FFWriter::open(std::shared_ptr<URL> url,
                    std::shared_ptr<CodecParams> audioCodecParams,
                    std::shared_ptr<CodecParams> videoCodecParams) {
    _url = url;
    _muxer = std::make_shared<FFMuxer>();
    if (!_muxer->open(_url, audioCodecParams, videoCodecParams)) {
        LOG(ERROR) << "Edit muxer open failed:" << _url;
        _muxer->close();
        _muxer = nullptr;
        return false;
    }
    return true;
}

bool FFWriter::close() {
    if (_muxer) {
        _muxer->close();
    }
    return true;
}

void FFWriter::flush() {
    if (_muxer) {
        _muxer->flush();
    }
}

bool FFWriter::cancel() {
    return close();
}

void FFWriter::processPacket(std::shared_ptr<Packet> packet) {
    if (_muxer) {
        _muxer->write(packet);
    }
}

void FFWriter::processFrame(std::shared_ptr<Frame> frame) {
    if (_muxer) {
        _muxer->write(frame);
    }
}

void FFWriter::process(int64_t timestamp) {
    
}
