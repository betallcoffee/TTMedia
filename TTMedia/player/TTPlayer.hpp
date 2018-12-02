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

#include "TTAudioQueue.hpp"
#include "TTRender.hpp"
#include "TTY420ToRGBFilter.hpp"

namespace TT {
    class Packet;
    class Frame;
    
    class DemuxerControl;
    class PlayerDemuxerObserver;
    
    class CodecControl;
    
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
        
        void bindRenderContext(const RenderContext &context);
        void bindAudioQueue(std::shared_ptr<AudioQueue> audioQueue);
        
        void bindFilter(std::shared_ptr<Filter> filter);
        
        friend PlayerDemuxerObserver;
        
    private:
        void setStatus(ePlayerStatus status);
        void waitStatusChange();
        bool open();
        bool openAudio();
        bool openVideo();
        bool close();
        
        void quit();
        bool isQuit();
        
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
        static void *videoThreadEntry(void *arg);
        void videoLoop();
        
        static void *audioThreadEntry(void *arg);
        void audioLoop();
        
        static void *renderThreadEntry(void *arg);
        void renderLoop();
        
        void audioCodecCB(AudioDesc &desc);
        std::shared_ptr<Frame> audioQueueCB();
        
        void setMasterSyncType(AVSyncClock clock);
        double getMasterClock();
        
    private:
        std::shared_ptr<URL> _url;
        
        ePlayerStatus _status;
        pthread_cond_t _statusCond;
        pthread_mutex_t _statusMutex;
        
        std::shared_ptr<DemuxerControl> _demuxerControl;
        std::shared_ptr<PlayerDemuxerObserver> _demuxerObserver;
        
        std::shared_ptr<CodecControl> _videoControl;
        std::shared_ptr<CodecControl> _audioControl;
        
        
        pthread_t _inputThread;
        pthread_cond_t _inputCond;
        pthread_mutex_t _inputMutex;
        
        pthread_t _renderThread;
        pthread_cond_t _renderCond;
        pthread_mutex_t _renderMutex;
        bool _renderring;
        
        std::shared_ptr<AudioQueue> _audioQueue;
        Render _render;
        Y420ToRGBFilter _filter;
        
        AVSyncClock _clock;
        Clock _aClock;
        Clock _vClock;
        Clock _eClock;
        int64_t _vPTS;
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
}

#endif /* TTPlayer_hpp */
