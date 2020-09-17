//
//  TTFrame.cpp
//  TTMedia
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <stdlib.h>

#include "easylogging++.h"

#include "TTFrame.hpp"

using namespace TT;

Frame::Frame() {
    memset(data, 0, sizeof(data));
    memset(lineSize, 0, sizeof(lineSize));
}

Frame::~Frame() {
    for (int i = 0; i < kNumOfPlanars; i++) {
        if (data[i]) {
            free(data[i]);
            data[i] = nullptr;
        }
    }
    if (_avFrame) {
        av_frame_free(&_avFrame);
        _avFrame = nullptr;
    }
}

Frame::Frame(AVFrame *avFrame, MediaType mediaType, DataType dataType)
: pts(avFrame->pts), pkt_pts(avFrame->pkt_pts), pkt_dts(avFrame->pkt_dts),
sampleFormat(AV_SAMPLE_FMT_NONE) {
    memset(data, 0, sizeof(data));
    memset(lineSize, 0, sizeof(lineSize));
    _mediaType = mediaType;
    if (avFrame) {
        _width = avFrame->width;
        _height = avFrame->height;
        
        _avFrame = av_frame_clone(avFrame);
        if (kMediaTypeVideo == _mediaType) {
            for (int i = 0; i < numOfPlanars; i++) {
                lineSize[i] = _avFrame->linesize[i];
                size_t dataSize = lineSize[i] * _height;
                if (i) dataSize /= 2;
                if (reallocData(dataSize, i)) {
                    memcpy(data[i], _avFrame->data[i], dataSize);
                } else {
                    LOG(ERROR) << "Frame init error linesize:" << lineSize[i] << " datasize:" << dataSize;
                }
            }
            _isKeyFrame = _avFrame->key_frame;
        }
    }
}

Frame::Frame(const uint8_t *srcData, size_t srcDataSize, MediaType mediaType, DataType dataType)
{
    memset(data, 0, sizeof(data));
    memset(lineSize, 0, sizeof(lineSize));
    
    _mediaType = mediaType;
    _dataType = dataType;
    
    reallocData(srcDataSize, 0);
    if (data[0] && srcData) {
        memcpy(data[0], srcData, srcDataSize);
    }
}

bool Frame::reallocData(size_t dataSize, int index) {
    if (dataSize <= 0 || index < 0 || index >= kNumOfPlanars) {
        LOG(ERROR) << "Frame realloc data size error:" << dataSize << " index:" << index;
        return false;
    }
    
    if (data[index]) {
        free(data[index]);
        data[index] = nullptr;
    }
    
    data[index] = (uint8_t *)malloc(dataSize);
    if (data[index] == nullptr) {
        LOG(ERROR) << "Frame realloc memory failed:" << dataSize << " index:" << index;
        return false;
    }
    
    return true;
}

bool Frame::isKeyframe() {
    return _isKeyFrame;
}
