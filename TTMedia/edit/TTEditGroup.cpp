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
#include "TTVideo.hpp"

using namespace TT;

EditGroup::EditGroup() :
_editMutex(PTHREAD_MUTEX_INITIALIZER) {
    _messageLoop = std::make_shared<MessageLoop>("Edit Group");
    _messageLoop->setMessageHandle(std::bind(&EditGroup::handleMessage, this, std::placeholders::_1));
}

EditGroup::~EditGroup() {
    _messageLoop->stop();
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
    Mutex m(&_editMutex);
    _materials.push_back(edit);
    if (MaterialType::kVideo == edit->type()) {
        std::shared_ptr<Video> video = std::dynamic_pointer_cast<Video>(edit);
        _videos.push_back(video);
    }
    _messageLoop->postMessage(std::make_shared<Message>(EditMessage::kProccess));
}

void EditGroup::removeMaterial(int index) {
    Mutex m(&_editMutex);
    if (index >= 0 && index < _materials.size()) {
        _materials.erase(_materials.begin() + index);
    }
}

void EditGroup::exportFile(std::shared_ptr<URL> url) {
    Mutex m(&_editMutex);
    _exportUrl = url;
    _messageLoop->postMessage(std::make_shared<Message>(EditMessage::kExportFile));
}

void EditGroup::quit() {
    Mutex m(&_editMutex);
    _messageLoop->stop();
}

void EditGroup::handleMessage(std::shared_ptr<Message> message) {
    Mutex m(&_editMutex);
    EditMessage code = static_cast<EditMessage>(message->code());
    if (EditMessage::kProccess == code) {
        if (!_materials.empty()) {
            std::for_each(_materials.begin(), _materials.end(), [&](std::shared_ptr<Material> material) {
                if (material) {
                    if (!material->process()) {
                        usleep(1);
                    }
                }
            });
        }
        _messageLoop->postMessage(std::make_shared<Message>(EditMessage::kProccess));
    } else if (EditMessage::kExportFile == code) {
        if (!_materials.empty()) {
            size_t width = 1024;
            size_t height = 720;
            if (!_videos.empty()) {
                std::shared_ptr<Video> video = *_videos.begin();
                width = video->width();
                height = video->height();
            }
            if (_writer == nullptr) {
                _writer = std::make_shared<FFWriter>();
                std::shared_ptr<CodecParams> videoCodecParams = CodecParams::H264CodecParams(width, height);
                if (!_writer->open(_exportUrl, nullptr, videoCodecParams)) {
                    LOG(ERROR) << "Edit muxer open failed:" << _exportUrl;
                    _writer->cancel();
                    _writer = nullptr;
                    return;
                }
            }
            
            if (_writer) {
                std::for_each(_materials.begin(), _materials.end(), [&](std::shared_ptr<Material> material) {
                    LOG(INFO) << "Writer index:" << material->startIndex << " to " << material->endIndex;
                    for (int i = material->startIndex; i < material->endIndex; i++) {
                        std::shared_ptr<Frame> frame = material->frame(i);
                        _writer->processFrame(frame);
                    }
                });
                _writer->flush();
            }
        }
    }
}




