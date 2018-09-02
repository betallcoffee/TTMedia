//
//  TTMP4Demuxer.hpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTMP4Demuxer_hpp
#define TTMP4Demuxer_hpp

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "mp4_bits.h"
#include "mp4_boxes.h"
#ifdef __cplusplus
}
#endif

#include "TTDemuxer.hpp"

namespace TT {
    
    class IO;
    
    class MP4Demuxer : public Demuxer {
    public:
        MP4Demuxer();
        ~MP4Demuxer();
        
        static int probe(std::shared_ptr<URL> url);
        
        bool open(std::shared_ptr<URL> url) override;
        void close() override;
        
        std::shared_ptr<Packet> read() override;
        
        bool seek(uint64_t pos) override;
        bool isEOF() override { return _isEOF; };
        
        bool hasAudio() override { return _audioStream == nullptr ? false : true; }
        bool hasVideo() override { return _videoStream == nullptr ? false : true; }
        
        std::shared_ptr<AudioCodec> audioCodec() override;
        std::shared_ptr<VideoCodec> videoCodec() override;
        
    private:
        bool parseBox();
        int64_t parseBoxHeader(struct mp4_box **box, mp4_bits_t *bs);
        
        std::shared_ptr<URL> _url;
        std::shared_ptr<IO> _io;
        
        pthread_mutex_t _mutex;
        bool _isEOF = false;
        
        struct mp4_bits *_bitStream;
        struct mp4_box *_newBox;
        
        /* moov box */
        struct mp4_moov_box *_moov;
        /* mdat box (one and only one when we store the file) */
        struct mp4_mdat_box *_mdat;
        /* file brand (since v2, NULL means mp4 v1) */
        struct mp4_ftyp_box *_brand;
        
        /* link all the root boxes */
        mp4_list_t *_rootBoxes;
    };
}



#endif /* TTMP4Demuxer_hpp */
