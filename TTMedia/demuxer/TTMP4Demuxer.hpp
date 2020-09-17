//
//  TTMP4Demuxer.hpp
//  TTMedia
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
        
    private:
        static uint8_t readByte(mp4_bits_t *bs);
        uint8_t readByte();
        static uint32_t readData(mp4_bits_t *bs, char *data, uint32_t nbBytes);
        uint32_t readData(char *data, uint32_t nbBytes);
        static int seekOffset(mp4_bits_t *bs, uint64_t offset);
        int seekOffset(uint64_t offset);
        static uint64_t readAvailable(mp4_bits_t *bs);
        uint64_t readAvailable();
        
        bool parseBox();
        int64_t parseBoxHeader(struct mp4_box **box, mp4_bits_t *bs);
        bool parseBoxBody();
        
        void dumpStreamInfo();
        void dumpStreamAudioInfo();
        void dumpStreamVideoInfo();
        
        std::shared_ptr<Packet> readAudio();
        std::shared_ptr<Packet> readVideo();
        
        bool readPacketData(std::shared_ptr<Packet> packet, size_t size);
        
        std::shared_ptr<URL> _url;
        std::shared_ptr<IO> _io;
        
        pthread_mutex_t _mutex;
        bool _isEOF = false;
        
        mp4_bits_t *_bitStream;
        struct mp4_box *_newBox;
        int64_t _newBoxSize;
        
        /* moov box */
        struct mp4_moov_box *_moov;
        /* mdat box (one and only one when we store the file) */
        struct mp4_mdat_box *_mdat;
        /* file brand (since v2, NULL means mp4 v1) */
        struct mp4_ftyp_box *_brand;
        
        /* link all the root boxes */
        mp4_list_t *_rootBoxes;
        
        int _audioTrackIdx;
        uint32_t _audioTrackId;
        uint32_t _audioNextSampleIdx;
        uint64_t _audioNextOffset;
        int _videoTrackIdx;
        uint32_t _videoTrackId;
        uint32_t _videoNextSampleIdx;
        uint64_t _videoNextOffset;
        
    };
}

#endif /* TTMP4Demuxer_hpp */
