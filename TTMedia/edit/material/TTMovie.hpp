//
//  TTMovie.hpp
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

#include "TTMaterial.hpp"
#include "TTFilterFrame.hpp"
#include "TTAudioQueue.hpp"
#include "TTRender.hpp"

namespace TT {

class Packet;
class Stream;
class Demuxer;
class FFWriter;
class AudioCodec;
class VideoCodec;

class Movie : public Material {
public:
    Movie();
    ~Movie();
    
    virtual bool open() override;
    virtual bool close() override;
    void save(std::shared_ptr<URL> url) override;
    bool loadMore() override;
    
    int audioFrameCount();
    std::shared_ptr<Frame> audioFrame(int index);
    
    virtual int frameCount() override;
    virtual TT_SP(Frame) frameAt(int index) override;
    
    int previewCount();
    std::shared_ptr<Frame> preview(int index);
    
private:
    bool readData();
    bool writeData();
    
    bool audioDecode(std::shared_ptr<Packet> packet);
    bool videoDecode(std::shared_ptr<Packet> packet);
    
private:
    std::shared_ptr<URL> _saveUrl = nullptr;
    
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
