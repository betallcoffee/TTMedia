//
//  TTPlayer.hpp
//  TTPlayerExample
//
//  Created by liang on 8/21/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTPlayer_hpp
#define TTPlayer_hpp

#include <pthread.h>

#include "TTdef.h"
#include "TTQueue.hpp"
#include "TTURL.hpp"
#include "TTCond.hpp"
#include "TTClock.hpp"

#include "TTDemuxerControl.hpp"
#include "TTCodecControl.hpp"
#include "TTRenderControl.hpp"

#include "TTAudioQueue.hpp"
#include "TTRender.hpp"
#include "TTY420ToRGBFilter.hpp"

namespace TT {
    class Packet;
    class Frame;
    
    class RenderContext;
    
    class PlayerDemuxerObserver;
    class VideoCodecObserver;
    
    typedef enum {
        kPlayerNone,
        kPlayerError,
        kPlayerOpen,
        kPlayerClose,
        kPlayerPlaying,
        kPlayerPaused,
        kPlayerStoped,
        kPlayerQuit,
    }ePlayerStatus;
    
    class Player : public std::enable_shared_from_this<Player> {
    public:
        Player();
        ~Player();
        
        void play(std::shared_ptr<URL> url);
        void stop();
        
        void pause();
        void resume();
        
        void seek();
        
        void bindRenderContext(std::shared_ptr<RenderContext> context);
        void bindAudioQueue(std::shared_ptr<AudioQueue> audioQueue);
        
        void bindFilter(std::shared_ptr<Filter> filter);
        
        friend PlayerDemuxerObserver;
        friend VideoCodecObserver;
        
    private:
        void setStatus(ePlayerStatus status);
        void waitStatusChange();
        
        bool open();
        bool openAudio();
        bool openVideo();
        bool openRender();
        
        bool close();
        
        void quit();
        bool isQuit();
        
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
        void audioCodecCB(AudioDesc &desc);
        std::shared_ptr<Frame> audioQueueCB();
        
        void setMasterSyncType(AVSyncClock clock);
        
    private:
        std::shared_ptr<URL> _url;
        
        ePlayerStatus _status;
        pthread_cond_t _statusCond;
        pthread_mutex_t _statusMutex;
        
        std::shared_ptr<DemuxerControl> _demuxerControl;
        std::shared_ptr<PlayerDemuxerObserver> _demuxerObserver;
        
        std::shared_ptr<CodecControl> _videoControl;
        std::shared_ptr<CodecObserver> _videoObserver;
        
        std::shared_ptr<CodecControl> _audioControl;
        std::shared_ptr<CodecObserver> _audioObserver;
        
        std::shared_ptr<RenderControl> _renderControl;
        std::shared_ptr<RenderContext> _renderContext;
        
        std::shared_ptr<AudioQueue> _audioQueue;
        std::shared_ptr<Filter> _bindFilter;
        
        AVSyncClock _clock;
        
        pthread_t _inputThread;
        pthread_cond_t _inputCond;
        pthread_mutex_t _inputMutex;
    };
    
    class PlayerDemuxerObserver : public DemuxerObserver {
    public:
        PlayerDemuxerObserver() {};
        ~PlayerDemuxerObserver() {};
        
        virtual void opened() override;
        virtual void closed() override;
        virtual void error() override;
        
        TT_PROPERTY_DEF(std::weak_ptr<Player>, player);
    };
    
    class VideoCodecObserver : public CodecObserver {
    public:
        VideoCodecObserver() {};
        ~VideoCodecObserver() {};
        
        virtual void opened() override;
        virtual void closed() override;
        virtual void error() override;
        
        TT_PROPERTY_DEF(std::weak_ptr<Player>, player);
    };
}

#endif /* TTPlayer_hpp */
