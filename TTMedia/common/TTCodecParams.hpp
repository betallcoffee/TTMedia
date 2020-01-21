//
//  TTCodecParams.hpp
//  TTPlayerExample
//
//  Created by liang on 6/4/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTCodecParams_hpp
#define TTCodecParams_hpp

#include <stdint.h>
#include <string>

#include "TTCommon.h"

namespace TT {
    
    class CodecParams {
    public:
        CodecParams();
        ~CodecParams();
        
        static std::shared_ptr<CodecParams> AACCodecParams(uint32_t channels, uint32_t sampleRate);
        static std::shared_ptr<CodecParams> H264CodecParams(size_t width, size_t heigth);
        
    public:
        /**
         * General type of the encoded data.
         */
        MediaType codecType;
        /**
         * Specific type of the encoded data (the codec used).
         */
        CodecID codecID;
        /**
         * Additional information about the codec (corresponds to the AVI FOURCC).
         */
        uint32_t codecTag;
        
        /**
         * Extra binary data needed for initializing the decoder, codec-dependent.
         *
         * Must be allocated with av_malloc() and will be freed by
         * avcodec_parameters_free(). The allocated size of extradata must be at
         * least extradata_size + AV_INPUT_BUFFER_PADDING_SIZE, with the padding
         * bytes zeroed.
         */
        uint8_t *extradata;
        /**
         * Size of the extradata content in bytes.
         */
        int extradataSize;
        
        /**
         * - video: the pixel format, the value corresponds to enum AVPixelFormat.
         * - audio: the sample format, the value corresponds to enum AVSampleFormat.
         */
        int format;
        
        /**
         * The average bitrate of the encoded data (in bits per second).
         */
        int64_t bitRate;
        
        /**
         * This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identically 1.
         * This often, but not always is the inverse of the frame rate or field rate
         * for video. 1/time_base is not the average frame rate if the frame rate is not
         * constant.
         *
         * Like containers, elementary streams also can store timestamps, 1/time_base
         * is the unit in which these timestamps are specified.
         * As example of such codec time base see ISO/IEC 14496-2:2001(E)
         * vop_time_increment_resolution and fixed_vop_rate
         * (fixed_vop_rate == 0 implies that it is different from the framerate)
         *
         * - encoding: MUST be set by user.
         * - decoding: the use of this field for decoding is deprecated.
         *             Use framerate instead.
         */
        TTRational timeBase;
        
        /**
         * the number of pictures in a group of pictures, or 0 for intra_only
         * - encoding: Set by user.
         * - decoding: unused
         */
        int gopSize;
        
        /**
         * The number of bits per sample in the codedwords.
         *
         * This is basically the bitrate per sample. It is mandatory for a bunch of
         * formats to actually decode them. It's the number of bits for one sample in
         * the actual coded bitstream.
         *
         * This could be for example 4 for ADPCM
         * For PCM formats this matches bits_per_raw_sample
         * Can be 0
         */
        int bits_per_coded_sample;
        
        /**
         * This is the number of valid bits in each output sample. If the
         * sample format has more bits, the least significant bits are additional
         * padding bits, which are always 0. Use right shifts to reduce the sample
         * to its actual size. For example, audio formats with 24 bit samples will
         * have bits_per_raw_sample set to 24, and format set to AV_SAMPLE_FMT_S32.
         * To get the original sample use "(int32_t)sample >> 8"."
         *
         * For ADPCM this might be 12 or 16 or similar
         * Can be 0
         */
        int bits_per_raw_sample;
        
        /**
         * Codec-specific bitstream restrictions that the stream conforms to.
         */
        int profile;
        int level;
        
        /**
         * Video only. The dimensions of the video frame in pixels.
         */
        int width;
        int height;
        
        /**
         * Video only. The aspect ratio (width / height) which a single pixel
         * should have when displayed.
         *
         * When the aspect ratio is unknown / undefined, the numerator should be
         * set to 0 (the denominator may have any value).
         */
        //    AVRational sample_aspect_ratio;
        
        /**
         * Video only. The order of the fields in interlaced video.
         */
        //    enum AVFieldOrder                  field_order;
        
        /**
         * Video only. Additional colorspace characteristics.
         */
        //    enum AVColorRange                  color_range;
        //    enum AVColorPrimaries              color_primaries;
        //    enum AVColorTransferCharacteristic color_trc;
        //    enum AVColorSpace                  color_space;
        //    enum AVChromaLocation              chroma_location;
        
        /**
         * Video only. Number of delayed frames.
         */
        int video_delay;
        
        /**
         * Audio only. The channel layout bitmask. May be 0 if the channel layout is
         * unknown or unspecified, otherwise the number of bits set must be equal to
         * the channels field.
         */
        uint64_t channelLayout;
        /**
         * Audio only. The number of audio channels.
         */
        int      channels;
        /**
         * Audio only. The number of audio samples per second.
         */
        int      sampleRate;
        /**
         * Audio only. The number of bytes per coded audio frame, required by some
         * formats.
         *
         * Corresponds to nBlockAlign in WAVEFORMATEX.
         */
        int      block_align;
        /**
         * Audio only. Audio frame size, if known. Required by some formats to be static.
         */
        int      frameSize;
        
        /**
         * Audio only. The amount of padding (in samples) inserted by the encoder at
         * the beginning of the audio. I.e. this number of leading decoded samples
         * must be discarded by the caller to get the original audio without leading
         * padding.
         */
        int initial_padding;
        /**
         * Audio only. The amount of padding (in samples) appended by the encoder to
         * the end of the audio. I.e. this number of decoded samples must be
         * discarded by the caller from the end of the stream to get the original
         * audio without any trailing padding.
         */
        int trailing_padding;
        /**
         * Audio only. Number of samples to skip after a discontinuity.
         */
        int seek_preroll;
    };
}

#endif /* TTCodecParams_hpp */
