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

#include "TTMaterial.hpp"

namespace TT {
    
    class Packet;
    class Frame;
    class Stream;
    class FFDemuxer;
    class FFWriter;
    class AudioCodec;
    class VideoCodec;
    
    class Media : public Material {
    public:
        Media(std::shared_ptr<URL> url);
        ~Media();
        
        bool open() override;
        bool close() override;
        void save(std::shared_ptr<URL> url) override;
        bool loadMore() override;
        
        int audioFrameCount() override;
        std::shared_ptr<Frame> audioFrame(int index) override;
        
        int frameCount() override;
        std::shared_ptr<Frame> frame(int index) override;
        
        size_t width() { return _width; }
        size_t height() { return _height; }
        
        int previewCount();
        std::shared_ptr<Frame> preview(int index);
        
    private:
        bool readData();
        bool writeData();
        
        bool audioDecode(std::shared_ptr<Packet> packet);
        bool videoDecode(std::shared_ptr<Packet> packet);
        
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
        
        Array<std::shared_ptr<Frame>> _aFrameArray;
    };
}

#endif /* TTVideoEdit_hpp */
