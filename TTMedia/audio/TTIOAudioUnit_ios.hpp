//
//  TTIOAudioUnit_ios.hpp
//  TTMedia
//
//  Created by liang on 17/3/19.
//  Copyright © 2019年 tina. All rights reserved.
//

#ifndef TTIOAudioUnit_ios_hpp
#define TTIOAudioUnit_ios_hpp

#include <AudioToolbox/AudioToolbox.h>

namespace TT {
    class IOAudioUnit_ios {
    public:
        IOAudioUnit_ios();
        ~IOAudioUnit_ios();
        
        bool setup();
        bool teardown();
        
        bool start();
        bool stop();
        
        void setInputEnable(bool enable) { _inputEnable = enable; }
        void setOutputEnable(bool enable) { _outputEnable = enable; }
        
    private:
        bool setupCallback();
        bool setupStreamFormat();
        void audioComponentDescriptionMake(AudioComponentDescription *acd, OSType manufacturer, OSType type, OSType subtype);
        
        static OSStatus audioUnitInputCallback(void *inRefCon,
                                               AudioUnitRenderActionFlags *ioActionFlags,
                                               const AudioTimeStamp *inTimeStamp,
                                               UInt32 inBusNumber,
                                               UInt32 inNumberFrames,
                                               AudioBufferList *ioData);
        
    private:
        AudioUnit _audioUnit;
        bool _inputEnable = false;
        bool _outputEnable = false;
        
        double _currentSampleRate = 0;
        double _sampleRate = 0;
        
        int _maximumInputChannels = 0;
        int _numberOfInputChannels = 0;
        
        AudioStreamBasicDescription const kAudioDescription = {
            .mFormatID          = kAudioFormatLinearPCM,
            .mFormatFlags       = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved,
            .mChannelsPerFrame  = 2,
            .mBytesPerPacket    = sizeof(float),
            .mFramesPerPacket   = 1,
            .mBytesPerFrame     = sizeof(float),
            .mBitsPerChannel    = 8 * sizeof(float),
            .mSampleRate        = 0,
        };
    };
}

#endif /* TTIOAudioUnit_ios_hpp */
