//
//  TTVideo.hpp
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

#include "TTMaterial.hpp"

namespace TT {
    
    class Packet;
    class Frame;
    class Stream;
    class FFDemuxer;
    class FFWriter;
    class AudioCodec;
    class VideoCodec;
    
    typedef enum class VideoEvent {
        kNone,
        kReadEnd,
        kWriteEnd,
    } eEditEvent;
    
    class Video : public Material {
    public:
        Video();
        ~Video();
        
        bool init();
        
        bool process() override;
        
        bool open(std::shared_ptr<URL> url) override;
        bool close() override;
        void save(std::shared_ptr<URL> url) override;
        bool loadMore() override;
        
        int frameCount() override;
        std::shared_ptr<Frame> frame(int index) override;
        
        size_t width() { return _width; }
        size_t height() { return _height; }
        
        int previewCount();
        std::shared_ptr<Frame> preview(int index);
        
        typedef std::function<void(Video *, size_t size)> ReadFrameCallback;
        void setReadFrameCallback(ReadFrameCallback cb);
        
        typedef std::function<void(Video *, VideoEvent event)> EventCallback;
        void setEventCallback(EventCallback cb);
        
        bool openDemuxer();
        bool closeMedia();
        bool readData();
        bool writeData();
        
    private:
        bool videoDecode(std::shared_ptr<Packet> packet);
        bool encode();
        
    private:
        std::shared_ptr<URL> _saveUrl;
        size_t _width;
        size_t _height;
        
        pthread_mutex_t _mutex;
        
        std::shared_ptr<Stream> _stream;
        std::shared_ptr<FFDemuxer> _demuxer;
        std::shared_ptr<FFWriter> _writer;
        
        std::shared_ptr<AudioCodec> _audioCodec;
        std::shared_ptr<VideoCodec> _videoCodec;
        
        Array<std::shared_ptr<Frame>> _vFrameArray;
        Array<std::shared_ptr<Frame>> _previews;
    };
}

#endif /* TTVideoEdit_hpp */
