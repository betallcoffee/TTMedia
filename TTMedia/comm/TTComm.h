//
//  TTComm.h
//  TTPlayerExample
//
//  Created by liang on 13/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTComm_h
#define TTComm_h

#include <sys/types.h>
#include "TTdef.h"

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
    
    /**
     * Rational number (pair of numerator and denominator).
     */
    typedef struct TTRational{
        uint32_t num; ///< Numerator
        uint32_t den; ///< Denominator
    } TTRational;
    
    /**
     * Convert an TTRational to a `double`.
     * @param a TTRational to convert
     * @return `a` in floating-point form
     */
    static inline double tt_q2d(TTRational a){
        return a.num / (double) a.den;
    }
    
#define TT_PROFILE_H264_BASELINE             66
#define TT_PROFILE_H264_CONSTRAINED_BASELINE (66|TT_PROFILE_H264_CONSTRAINED)
#define TT_PROFILE_H264_MAIN                 77
#define TT_PROFILE_H264_EXTENDED             88
#define TT_PROFILE_H264_HIGH                 100
#define TT_PROFILE_H264_HIGH_10              110
#define TT_PROFILE_H264_HIGH_10_INTRA        (110|TT_PROFILE_H264_INTRA)
#define TT_PROFILE_H264_MULTIVIEW_HIGH       118
#define TT_PROFILE_H264_HIGH_422             122
#define TT_PROFILE_H264_HIGH_422_INTRA       (122|TT_PROFILE_H264_INTRA)
#define TT_PROFILE_H264_STEREO_HIGH          128
#define TT_PROFILE_H264_HIGH_444             144
#define TT_PROFILE_H264_HIGH_444_PREDICTIVE  244
#define TT_PROFILE_H264_HIGH_444_INTRA       (244|TT_PROFILE_H264_INTRA)
#define TT_PROFILE_H264_CAVLC_444            44
    
}


#endif /* TTComm_h */
