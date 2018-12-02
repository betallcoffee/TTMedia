//
//  TTRenderControl.hpp
//  TTPlayerExample
//
//  Created by liang on 2/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTRenderControl_hpp
#define TTRenderControl_hpp

#include <memory>
#include "TTdef.h"
#include "TTQueue.hpp"

#include "TTClock.hpp"

#include "TTRender.hpp"
#include "TTY420ToRGBFilter.hpp"

namespace TT {
    class MessageLoop;
    class Message;
    
    class Frame;
    
    typedef enum RenderMessage {
        kRenderOpen,
        kRenderRender,
        kRenderClose,
    } RenderMessage;
    
    class RenderControl {
    public:
        RenderControl();
        ~RenderControl();
        
        bool start();
        bool stop();
        void updateAudioClock(int64_t pts);
        
        TT_PROPERTY_DEF_READONLY(std::shared_ptr<MessageLoop>, loop);
        
        typedef Queue<std::shared_ptr<Frame>> FrameQueue;
        TT_PROPERTY_DEF(std::shared_ptr<FrameQueue>, frameQueue);
        
        TT_PROPERTY_DEF(std::shared_ptr<Y420ToRGBFilter>, filter);
        
        TT_PROPERTY_DEF(AVSyncClock, clock);
        
    private:
        void initMessages();
        void handleMessage(std::shared_ptr<Message> message);
        
        void render();
        
        void setMasterSyncType(AVSyncClock clock);
        double getMasterClock();
        
        Render _render;
        
        Clock _aClock;
        Clock _vClock;
        Clock _eClock;
        int64_t _vPTS;
    };
}

#endif /* TTRenderControl_hpp */
