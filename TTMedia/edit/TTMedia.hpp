//
//  TTMedia.hpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTVideoEdit_hpp
#define TTVideoEdit_hpp

#include <pthread.h>
#include <vector>

#include "TTArray.hpp"
#include "TTQueue.hpp"
#include "TTURL.hpp"

#include "TTFilterFrame.hpp"
#include "TTAudioQueue.hpp"
#include "TTRender.hpp"

namespace TT {

class Packet;
class Frame;
class Stream;
class Demuxer;
class FFWriter;
class AudioCodec;
class VideoCodec;

class Media {
public:
    Media(std::shared_ptr<URL> url);
    ~Media();
    
    bool open();
    bool close();
    void save(std::shared_ptr<URL> url);
    bool loadMore();
    
    int audioFrameCount();
    std::shared_ptr<Frame> audioFrame(int index);
    
    int frameCount();
    std::shared_ptr<Frame> frame(int index);
    
    size_t width() { return _width; }
    size_t height() { return _height; }
    
    int previewCount();
    std::shared_ptr<Frame> preview(int index);
    
    TT_PROPERTY_DEF_READONLY(bool, isOpen);
    TT_PROPERTY_DEF_READONLY(bool, isEnd);
    TT_PROPERTY_DEF_READONLY(std::shared_ptr<URL>, url);
    
private:
    bool readData();
    bool writeData();
    
    bool audioDecode(std::shared_ptr<Packet> packet);
    bool videoDecode(std::shared_ptr<Packet> packet);
    
private:
    std::shared_ptr<URL> _saveUrl = nullptr;
    size_t _width = 0;
    size_t _height = 0;
    
    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    
    std::shared_ptr<Stream> _stream = nullptr;
    std::shared_ptr<Demuxer> _demuxer = nullptr;
    std::shared_ptr<FFWriter> _writer = nullptr;
    
    std::shared_ptr<AudioCodec> _audioCodec = nullptr;
    std::shared_ptr<VideoCodec> _videoCodec = nullptr;
    
    Array<std::shared_ptr<Frame>> _vFrameArray;
    Array<std::shared_ptr<Frame>> _previews;
    
    Array<std::shared_ptr<Frame>> _aFrameArray;
};
}

#endif /* TTVideoEdit_hpp */
