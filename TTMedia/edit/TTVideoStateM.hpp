//
//  TTVideoStateM.hpp
//  TTPlayerExample
//
//  Created by liang on 16/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTVideoStateM_hpp
#define TTVideoStateM_hpp

#include <memory>

#include "TTdef.h"
#include "TTStateM.hpp"

namespace TT {
    class Video;
    
    enum class VideoStateEvent {
        kNone,
        kRun,
        kOpen,
        kClose,
        kReady,
        kSave,
    };
    
    class VideoState : public State {
    public:
        VideoState(std::shared_ptr<Video> video);
        virtual ~VideoState() {};
        TT_PROPERTY_DEF_READONLY(std::weak_ptr<Video>, video);
    };
    
    class VideoStateInit : public VideoState {
    public:
        VideoStateInit(std::shared_ptr<Video> video);
        ~VideoStateInit() {};
    
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
        
        int run() override;
    };
    
    class VideoStateRead : public VideoState {
    public:
        VideoStateRead(std::shared_ptr<Video> video);
        ~VideoStateRead() {};
     
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
        
        int run() override;
    };
    
    class VideoStateEdit : public VideoState {
    public:
        VideoStateEdit(std::shared_ptr<Video> video);
        ~VideoStateEdit() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
        
        int run() override;
    };
    
    class VideoStateWrite : public VideoState {
    public:
        VideoStateWrite(std::shared_ptr<Video> video);
        ~VideoStateWrite() {};
        
    protected:
        bool enter() override;
        bool leave() override;
        std::shared_ptr<State> slot(int signal) override;
        
        int run() override;
    };
    
    class VideoStateM : public StateM {
    public:
        VideoStateM(std::shared_ptr<Video> video);
        ~VideoStateM();
        
    private:
        std::weak_ptr<Video> _video;
    };
}

#endif /* TTVideoStateM_hpp */
