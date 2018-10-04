//
//  TTComm.h
//  TTPlayerExample
//
//  Created by liang on 13/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTComm_h
#define TTComm_h

#define TT_PROPERTY_DECL(type, var) private: type _##var; \
public: type var(); \
public: void set##var(type var)

#define TT_PROPERTY_DECL_READONLY(type, var) private: type _##var; \
public: type var(); \
private: void set##var(type var)

#define TT_PROPERTY_IMPL(class, type, var) type class::var() { return _##var; } \
void class::set##var(type var) { _##var = var; }

namespace TT {
    typedef enum {
        kTextureTypeRGB = 1,
        kTextureTypeY420p = 2,
        kTextureTypeY420sp = 3,
        
        kAudioPCM = 4,
    } DataType;
    
    typedef enum {
        kMediaTypeNone = 0,
        kMediaTypeVideo = 1,
        kMediaTypeAudio = 2
    } MediaType;
    
    typedef enum {
        kPacketTypeNone = 0,
        kPacketTypeAudio = 1,
        kPacketTypeVideo = 2
    } PacketType;
    
    typedef enum {
        kCodecIDNone = 0,
        kCodecIDAAC = 1,
        kCodecIDH264 = 2,
    } CodecID;
    
    typedef enum {
        kStreamTypeNone = 0,
        kStreamTypeAudio = 1,
        kStreamTypeVideo = 2,
        kStreamTypeSubtitle = 3,
    } StreamType;
}


#endif /* TTComm_h */
