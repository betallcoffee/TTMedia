//
//  TTEditSpace.cpp
//  TTMedia
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include <unistd.h>

#include "easylogging++.h"
#include "TTDef.h"
#include "TTMutex.hpp"
#include "TTEditSpace.hpp"
#include "TTMaterial.hpp"

using namespace TT;

EditSpace::EditSpace() :
_editMutex(PTHREAD_MUTEX_INITIALIZER) {
    _loop = TT_MK_SP(MessageLoop)("Edit Group");
}

EditSpace::~EditSpace() {
    _loop->stop();
}

int EditSpace::materialCount() {
    return (int)_materials.size();
}

TT_SP(Material) EditSpace::material(int index) {
    Mutex m(&_editMutex);
    if (0 <= index && index < _materials.size()) {
        return _materials[index];
    }
    return nullptr;
}

void EditSpace::addMaterial(std::shared_ptr<Material> material) {
    int code = static_cast<int>(EditMessage::kOpen);
    auto openFunc = [&, material](TT_SP(Message) message) {
        material->open();
    };
    auto openMessage = TT_MK_SP(Message)(code, openFunc);
    _loop->postMessage(openMessage);
    
    Mutex m(&_editMutex);
    _materials.push_back(material);
}

void EditSpace::removeMaterial(int index) {
    Mutex m(&_editMutex);
    if (index >= 0 && index < _materials.size()) {
        _materials.erase(_materials.begin() + index);
    }
}

void EditSpace::loadMoreForMaterial(std::shared_ptr<Material> material, Callback callback) {
    int code = static_cast<int>(EditMessage::kLoadMore);
    auto loadFunc = [=](TT_SP(Message) message) {
        Mutex m(&_editMutex);
        material->loadMore();
        if (callback) {
            callback();
        }
    };
    auto loadMessage = TT_MK_SP(Message)(code, loadFunc);
    _loop->postMessage(loadMessage);
}

void EditSpace::addClip(std::shared_ptr<Clip> clip) {
    Mutex m(&_editMutex);
    _clips.push_back(clip);
}

void EditSpace::removeClip(int index) {
    
}

void EditSpace::exportFile(std::shared_ptr<URL> url) {
    _exportUrl = url;
    int code = static_cast<int>(EditMessage::kExportFile);
    auto exportFunc = [&](std::shared_ptr<Message> message) {
        Mutex m(&_editMutex);
        if (!_clips.empty()) {
            this->createWriter();
            this->exportAllClips();
        }
    };
    auto exportMessage = TT_MK_SP(Message)(code, exportFunc);
    _loop->postMessage(exportMessage);
}

void EditSpace::quit() {
    Mutex m(&_editMutex);
    _loop->stop();
}

bool EditSpace::createWriter() {
    size_t width = 1024;
    size_t height = 720;
    if (!_materials.empty()) {
        std::shared_ptr<Material> video = *_materials.begin();
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
            return true;
        }
    }
    
    return false;
}

void EditSpace::exportAllClips() {
    if (_writer) {
        for (auto clip : _clips) {
            LOG(INFO) << "Writer index:" << clip->srcStartIndex() << " to " << clip->srcEndIndex();
            auto material = clip->material();
            for (int i = clip->srcStartIndex(); i < clip->srcEndIndex(); i++) {
                auto frame = material->frameAt(i);
                _writer->processFrame(frame);
            }
        }
        
        _writer->flush();
    }
}




