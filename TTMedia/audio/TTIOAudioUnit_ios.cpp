//
//  TTIOAudioUnit_ios.cpp
//  TTPlayerExample
//
//  Created by liang on 17/3/19.
//  Copyright © 2019年 tina. All rights reserved.
//

#include <algorithm>
#include "easylogging++.h"
#include "TTIOAudioUnit_ios.hpp"

using namespace TT;

void TTError(OSStatus result,
             const char * _Nonnull operation,
             const char * _Nonnull file,
             int line) {
    int fourCC = CFSwapInt32HostToBig(result);
    if (isascii(((char*)&fourCC)[0]) &&
        isascii(((char*)&fourCC)[1]) &&
        isascii(((char*)&fourCC)[2]) ) {
        LOG(ERROR) << file << ":" << line << ":" << operation << ":" << (char*)&fourCC;
    } else {
        LOG(ERROR) << file << ":" << line << ":" << operation << ":" << (int)result;
    }
}

/*!
 * Check an OSStatus condition
 *
 * @param result The result
 * @param operation A description of the operation, for logging purposes
 */
#define TTCheckOSStatus(result,operation) (_TTCheckOSStatus((result),(operation),strrchr(__FILE__, '/')+1,__LINE__))
static inline bool _TTCheckOSStatus(OSStatus result,
                                    const char * _Nonnull operation,
                                    const char * _Nonnull file,
                                    int line) {
    if (result != noErr) {
        TTError(result, operation, file, line);
        return false;
    }
    return true;
}

IOAudioUnit_ios::IOAudioUnit_ios() {
    
}

IOAudioUnit_ios::~IOAudioUnit_ios() {
    
}

bool IOAudioUnit_ios::setup() {
    // Get an instance of the output audio unit
    AudioComponentDescription acd = {};
    audioComponentDescriptionMake(&acd, kAudioUnitManufacturer_Apple,
                                  kAudioUnitType_Output,
                                  kAudioUnitSubType_RemoteIO);
    
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &acd);
    OSStatus result = AudioComponentInstanceNew(inputComponent, &_audioUnit);
    if (!TTCheckOSStatus(result, "AudioComponentInstanceNew")) {
        return false;
    }
    
    int flag = _inputEnable ? 1 : 0;
    result = AudioUnitSetProperty(_audioUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Input, 1,
                                  &flag, sizeof(flag));
    if (!TTCheckOSStatus(result, "AudioUnitSetProperty(kAudioOutputUnitProperty_EnableIO)")) {
        return false;
    }
    
    flag = _outputEnable ? 1 : 0;
    result = AudioUnitSetProperty(_audioUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Output, 0,
                                  &flag, sizeof(flag));
    if (!TTCheckOSStatus(result, "AudioUnitSetProperty(kAudioOutputUnitProperty_EnableIO)")) {
        return false;
    }
    
    if (!setupCallback()) {
        return false;
    }
    
    if (!setupStreamFormat()) {
        return false;
    }
    
    return true;
}

bool IOAudioUnit_ios::teardown() {
    return false;
}

bool IOAudioUnit_ios::start() {
    OSStatus result = AudioOutputUnitStart(_audioUnit);
    if (!TTCheckOSStatus(result, "AudioOutputUnitStart")) {
        return false;
    }
    return true;
}

bool IOAudioUnit_ios::stop() {
    TTCheckOSStatus(AudioOutputUnitStop(_audioUnit), "AudioOutputUnitStop");
    return true;
}

bool IOAudioUnit_ios::setupCallback() {
    // Set the input callback
    AURenderCallbackStruct inRenderProc;
    inRenderProc.inputProc = &IOAudioUnit_ios::audioUnitInputCallback;
    inRenderProc.inputProcRefCon = this;
    OSStatus result = AudioUnitSetProperty(_audioUnit,
                                  kAudioOutputUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Global,
                                  0, &inRenderProc, sizeof(inRenderProc));
    if (!TTCheckOSStatus(result, "AudioUnitSetProperty(kAudioOutputUnitProperty_SetInputCallback)")) {
        return false;
    }
    
    return true;
}

bool IOAudioUnit_ios::setupStreamFormat() {
    bool hasChanges = false;
    bool iaaInput = false;
    
    if (_inputEnable) {
        // Get the current input number of input channels
        AudioStreamBasicDescription asbd;
        UInt32 size = sizeof(asbd);
        TTCheckOSStatus(AudioUnitGetProperty(_audioUnit,
                                             kAudioUnitProperty_StreamFormat,
                                             kAudioUnitScope_Input,
                                             1, &asbd, &size),
                        "AudioUnitGetProperty(kAudioUnitProperty_StreamFormat)");
        
        if (iaaInput) {
            asbd.mChannelsPerFrame = 2;
        }
        
        int channels = _maximumInputChannels ? std::min((int)asbd.mChannelsPerFrame, _maximumInputChannels) : asbd.mChannelsPerFrame;
        if ( _numberOfInputChannels != (int)channels ) {
            hasChanges = true;
            _numberOfInputChannels = channels;
        }
        
        if (!_outputEnable) {
            _currentSampleRate = _sampleRate == 0 ? asbd.mSampleRate : _sampleRate;
        }
        
        if ( _numberOfInputChannels > 0 ) {
            // Set the stream format
            asbd = kAudioDescription;
            asbd.mChannelsPerFrame = _numberOfInputChannels;
            asbd.mSampleRate = _currentSampleRate;
            TTCheckOSStatus(AudioUnitSetProperty(_audioUnit,
                                                 kAudioUnitProperty_StreamFormat,
                                                 kAudioUnitScope_Output,
                                                 1, &asbd, sizeof(asbd)),
                            "AudioUnitSetProperty(kAudioUnitProperty_StreamFormat)");
        } else {
//            memset(&_inputTimestamp, 0, sizeof(_inputTimestamp));
        }
    }
    
    return true;
}

void IOAudioUnit_ios::audioComponentDescriptionMake(AudioComponentDescription *acd,
                                                    OSType manufacturer,
                                                    OSType type, OSType subtype) {
    if (acd) {
        memset(acd, 0, sizeof(AudioComponentDescription));
        acd->componentManufacturer = manufacturer;
        acd->componentType = type;
        acd->componentSubType = subtype;
    }
}

OSStatus IOAudioUnit_ios::audioUnitInputCallback(void *inRefCon,
                                                 AudioUnitRenderActionFlags *ioActionFlags,
                                                 const AudioTimeStamp *inTimeStamp,
                                                 UInt32 inBusNumber,
                                                 UInt32 inNumberFrames,
                                                 AudioBufferList *ioData) {
    // Grab timestamp
//    __unsafe_unretained AEIOAudioUnit * THIS = (__bridge AEIOAudioUnit *)inRefCon;
//
//    AudioTimeStamp timestamp = *inTimeStamp;
//
//#if TARGET_OS_IPHONE
//    if ( THIS->_latencyCompensation ) {
//        timestamp.mHostTime -= AEHostTicksFromSeconds(THIS->_inputLatency);
//    }
//#endif
//
//    THIS->_inputTimestamp = timestamp;
    return noErr;
}
