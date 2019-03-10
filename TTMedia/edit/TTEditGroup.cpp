//
//  TTEditGroup.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include <unistd.h>

#include "easylogging++.h"

#include "TTMutex.hpp"
#include "TTEditGroup.hpp"
#include "TTMedia.hpp"

using namespace TT;

EditGroup::EditGroup() :
_editMutex(PTHREAD_MUTEX_INITIALIZER) {
    _loop = std::make_shared<MessageLoop>("Edit Group");
}

EditGroup::~EditGroup() {
    _loop->stop();
}

int EditGroup::materialCount() {
    return (int)_materials.size();
}

std::shared_ptr<Material> EditGroup::material(int index) {
    Mutex m(&_editMutex);
    if (0 <= index && index < _materials.size()) {
        return _materials[index];
    }
    return nullptr;
}

void EditGroup::addMaterial(std::shared_ptr<Material> edit) {
    _loop->postMessage(std::make_shared<Message>(static_cast<int>(EditMessage::kOpen), [&, edit](std::shared_ptr<Message> message){
        edit->open();
    }));
    
    Mutex m(&_editMutex);
    _materials.push_back(edit);
    if (MaterialType::kVideo == edit->type()) {
        std::shared_ptr<Media> video = std::dynamic_pointer_cast<Media>(edit);
        _videos.push_back(video);
    }
}

void EditGroup::removeMaterial(int index) {
    Mutex m(&_editMutex);
    if (index >= 0 && index < _materials.size()) {
        _materials.erase(_materials.begin() + index);
    }
}

void EditGroup::loadMoreForMaterial(std::shared_ptr<Material> material, Callback callback) {
    _loop->postMessage(std::make_shared<Message>(static_cast<int>(EditMessage::kLoadMore), [=](std::shared_ptr<Message> message) {
        Mutex m(&_editMutex);
        material->loadMore();
        if (callback) {
            callback();
        }
    }));
}

void EditGroup::exportFile(std::shared_ptr<URL> url) {
    _exportUrl = url;
    _loop->postMessage(std::make_shared<Message>(static_cast<int>(EditMessage::kExportFile), [&](std::shared_ptr<Message> message) {
        Mutex m(&_editMutex);
        if (!_materials.empty()) {
            size_t width = 1024;
            size_t height = 720;
            if (!_videos.empty()) {
                std::shared_ptr<Media> video = *_videos.begin();
                width = video->width();
                height = video->height();
            }
            if (_writer == nullptr) {
                _writer = std::make_shared<FFWriter>();
                std::shared_ptr<CodecParams> audioCodecParams = CodecParams::AACCodecParams(1, 44100);
                std::shared_ptr<CodecParams> videoCodecParams = CodecParams::H264CodecParams(width, height);
                if (!_writer->open(_exportUrl, audioCodecParams, videoCodecParams)) {
                    LOG(ERROR) << "Edit muxer open failed:" << _exportUrl;
                    _writer->cancel();
                    _writer = nullptr;
                    return;
                }
            }
            
            if (_writer) {
                std::for_each(_materials.begin(), _materials.end(), [&](std::shared_ptr<Material> material) {
                    LOG(INFO) << "Writer index:" << material->startIndex << " to " << material->endIndex;
//                    for (int i = material->startIndex; i < material->endIndex; i++) {
//                        std::shared_ptr<Frame> frame = material->frame(i);
//                        _writer->processFrame(frame);
//                    }
                    int count = material->audioFrameCount();
                    for (int i = 0; i < count; i++) {
                        std::shared_ptr<Frame> frame = material->audioFrame(i);
                        _writer->processFrame(frame);
                    }
                });
                _writer->flush();
            }
        }
    }));
}

void EditGroup::quit() {
    Mutex m(&_editMutex);
    _loop->stop();
}




