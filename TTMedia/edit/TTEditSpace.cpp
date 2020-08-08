//
//  TTEditSpace.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include <unistd.h>

#include "easylogging++.h"
#include "TTDef.h"
#include "TTMutex.hpp"
#include "TTEditSpace.hpp"
#include "TTMedia.hpp"

using namespace TT;

EditSpace::EditSpace() :
_editMutex(PTHREAD_MUTEX_INITIALIZER) {
    _loop = TT_MK_SP(MessageLoop)("Edit Group");
}

EditSpace::~EditSpace() {
    _loop->stop();
}

int EditSpace::mediaCount() {
    return (int)_medias.size();
}

TT_SP(Media) EditSpace::media(int index) {
    Mutex m(&_editMutex);
    if (0 <= index && index < _medias.size()) {
        return _medias[index];
    }
    return nullptr;
}

void EditSpace::addMedia(std::shared_ptr<Media> media) {
    int code = static_cast<int>(EditMessage::kOpen);
    auto openFunc = [&, media](TT_SP(Message) message) {
        media->open();
    };
    auto openMessage = TT_MK_SP(Message)(code, openFunc);
    _loop->postMessage(openMessage);
    
    Mutex m(&_editMutex);
    _medias.push_back(media);
}

void EditSpace::removeMedia(int index) {
    Mutex m(&_editMutex);
    if (index >= 0 && index < _medias.size()) {
        _medias.erase(_medias.begin() + index);
    }
}

void EditSpace::loadMoreForMedia(std::shared_ptr<Media> media, Callback callback) {
    int code = static_cast<int>(EditMessage::kLoadMore);
    auto loadFunc = [=](TT_SP(Message) message) {
        Mutex m(&_editMutex);
        media->loadMore();
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
    if (!_medias.empty()) {
        std::shared_ptr<Media> video = *_medias.begin();
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
            auto media = clip->media();
            for (int i = clip->srcStartIndex(); i < clip->srcEndIndex(); i++) {
                auto frame = media->frame(i);
                _writer->processFrame(frame);
            }
        }
        
        _writer->flush();
    }
}




