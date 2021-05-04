//
//  TTMP4Box.hpp
//  TTMedia
//
//  Created by liang on 20/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTMP4Box_hpp
#define TTMP4Box_hpp

#include <stdint.h>
#include <vector>
#include <string>

#include "TTDef.h"
#include "TTMP4.hpp"

namespace TT {

enum {
    /* 14496-12 */
    MP4_BOX_TYPE_co64 = MP4_FOURCC('c', 'o', '6', '4'),
    MP4_BOX_TYPE_cprt = MP4_FOURCC('c', 'p', 'r', 't'),
    MP4_BOX_TYPE_ctts = MP4_FOURCC('c', 't', 't', 's'),

    MP4_BOX_TYPE_dinf = MP4_FOURCC('d', 'i', 'n', 'f'),
    MP4_BOX_TYPE_dref = MP4_FOURCC('d', 'r', 'e', 'f'),

    MP4_BOX_TYPE_edts = MP4_FOURCC('e', 'd', 't', 's'),
    MP4_BOX_TYPE_elst = MP4_FOURCC('e', 'l', 's', 't'),

    MP4_BOX_TYPE_free = MP4_FOURCC('f', 'r', 'e', 'e'),
    MP4_BOX_TYPE_ftyp = MP4_FOURCC('f', 't', 'y', 'p'),

    MP4_BOX_TYPE_hdlr = MP4_FOURCC('h', 'd', 'l', 'r'),
    MP4_BOX_TYPE_hmhd = MP4_FOURCC('h', 'm', 'h', 'd'),

    MP4_BOX_TYPE_mdat = MP4_FOURCC('m', 'd', 'a', 't'),
    MP4_BOX_TYPE_mdhd = MP4_FOURCC('m', 'd', 'h', 'd'),
    MP4_BOX_TYPE_mdia = MP4_FOURCC('m', 'd', 'i', 'a'),
    MP4_BOX_TYPE_mehd = MP4_FOURCC('m', 'e', 'h', 'd'),
    MP4_BOX_TYPE_meta = MP4_FOURCC('m', 'e', 't', 'a'),
    MP4_BOX_TYPE_minf = MP4_FOURCC('m', 'i', 'n', 'f'),
    MP4_BOX_TYPE_moov = MP4_FOURCC('m', 'o', 'o', 'v'),
    MP4_BOX_TYPE_mvex = MP4_FOURCC('m', 'v', 'e', 'x'),
    MP4_BOX_TYPE_mvhd = MP4_FOURCC('m', 'v', 'h', 'd'),

    MP4_BOX_TYPE_nmhd = MP4_FOURCC('n', 'm', 'h', 'd'),

    MP4_BOX_TYPE_padb = MP4_FOURCC('p', 'a', 'd', 'b'),

    MP4_BOX_TYPE_rtp = MP4_FOURCC('r', 't', 'p', ' '),

    MP4_BOX_TYPE_sbpg = MP4_FOURCC('s', 'b', 'p', 'g'),
    MP4_BOX_TYPE_sdtp = MP4_FOURCC('s', 'd', 't', 'p'),
    MP4_BOX_TYPE_sgpd = MP4_FOURCC('s', 'g', 'p', 'd'),
    MP4_BOX_TYPE_smhd = MP4_FOURCC('s', 'm', 'h', 'd'),
    MP4_BOX_TYPE_skip = MP4_FOURCC('s', 'k', 'i', 'p'),
    MP4_BOX_TYPE_stbl = MP4_FOURCC('s', 't', 'b', 'l'),
    MP4_BOX_TYPE_stco = MP4_FOURCC('s', 't', 'c', 'o'),
    MP4_BOX_TYPE_stdp = MP4_FOURCC('s', 't', 'd', 'p'),
    MP4_BOX_TYPE_stsc = MP4_FOURCC('s', 't', 's', 'c'),
    MP4_BOX_TYPE_stsd = MP4_FOURCC('s', 't', 's', 'd'),
    MP4_BOX_TYPE_stsh = MP4_FOURCC('s', 't', 's', 'h'),
    MP4_BOX_TYPE_stss = MP4_FOURCC('s', 't', 's', 's'),
    MP4_BOX_TYPE_stsz = MP4_FOURCC('s', 't', 's', 'z'),
    MP4_BOX_TYPE_stz2 = MP4_FOURCC('s', 't', 'z', '2'),
    MP4_BOX_TYPE_stts = MP4_FOURCC('s', 't', 't', 's'),
    MP4_BOX_TYPE_snro = MP4_FOURCC('s', 'n', 'r', 'o'),
    MP4_BOX_TYPE_subs = MP4_FOURCC('s', 'u', 'b', 's'),

    MP4_BOX_TYPE_tims = MP4_FOURCC('t', 'i', 'm', 's'),
    MP4_BOX_TYPE_tkhd = MP4_FOURCC('t', 'k', 'h', 'd'),
    MP4_BOX_TYPE_trex = MP4_FOURCC('t', 'r', 'e', 'x'),
    MP4_BOX_TYPE_tref = MP4_FOURCC('t', 'r', 'e', 'f'),
    MP4_BOX_TYPE_trak = MP4_FOURCC('t', 'r', 'a', 'k'),
    MP4_BOX_TYPE_tsro = MP4_FOURCC('t', 's', 'r', 'o'),

    MP4_BOX_TYPE_udta = MP4_FOURCC('u', 'd', 't', 'a'),
    MP4_BOX_TYPE_url = MP4_FOURCC('u', 'r', 'l', ' '),
    MP4_BOX_TYPE_urn = MP4_FOURCC('u', 'r', 'n', ' '),
    MP4_BOX_TYPE_uuid = MP4_FOURCC('u', 'u', 'i', 'd'),

    MP4_BOX_TYPE_vmhd = MP4_FOURCC('v', 'm', 'h', 'd'),

    /* 14496-14 */
    MP4_BOX_TYPE_esds = MP4_FOURCC('e', 's', 'd', 's'),
    MP4_BOX_TYPE_iods = MP4_FOURCC('i', 'o', 'd', 's'),

    MP4_BOX_TYPE_mp4a = MP4_FOURCC('m', 'p', '4', 'a'),
    MP4_BOX_TYPE_mp4s = MP4_FOURCC('m', 'p', '4', 's'),
    MP4_BOX_TYPE_mp4v = MP4_FOURCC('m', 'p', '4', 'v'),

    /* 14496-15 */
    MP4_BOX_TYPE_avc1 = MP4_FOURCC('a', 'v', 'c', '1'),
    MP4_BOX_TYPE_avcC = MP4_FOURCC('a', 'v', 'c', 'C'),
    MP4_BOX_TYPE_avcp = MP4_FOURCC('a', 'v', 'c', 'p'),
    MP4_BOX_TYPE_btrt = MP4_FOURCC('b', 't', 'r', 't'),
    MP4_BOX_TYPE_m4ds = MP4_FOURCC('m', '4', 'd', 's'),

    /* todo */
    MP4_BOX_TYPE_todo = MP4_FOURCC('t', 'o', 'd', 'o')
};

class MP4Box {
public:
    MP4Box();
    ~MP4Box();
    
    // 1 表示 type 接着 Extended Size ，0 表示到文件末尾
    TT_PROPERTY_DEF_READONLY(uint64_t, size, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, type, 0);
    
    uint8_t _uuid[16] = {0};
    TT_PROPERTY_DEF_READONLY(uint32_t, index, 0);
};

class MP4FullBox {
public:
    MP4FullBox();
    ~MP4FullBox();
    
    TT_PROPERTY_DEF_READONLY(uint8_t, version, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, flags, 0);
};

/**
 file type and compatibility
 mandatory
 */
class MP4BoxFtyp : public MP4Box {
    TT_PROPERTY_DEF_READONLY(uint32_t, majorBrand, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, minorVersion, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, compatibleBrandsCount, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t *, compatibleBrands, nullptr);
};

/**
 container for all the metadata
 mandatory
 */
class MP4BoxMoov : public MP4Box {
    
    
};

/**
 movie header, overall declarations
 moov
 mandatory
 */
class MP4BoxMvhd : public MP4FullBox {
    TT_PROPERTY_DEF_READONLY(uint64_t, creationTime, 0);
    TT_PROPERTY_DEF_READONLY(uint64_t, modification, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, timeScale, 0);
    TT_PROPERTY_DEF_READONLY(uint64_t, duration , 0);
    
    TT_PROPERTY_DEF_READONLY(uint32_t, rate, 0);
    TT_PROPERTY_DEF_READONLY(uint16_t, volume, 0);
    
    TT_PROPERTY_DEF_READONLY(uint32_t, nextId, 0);
};

class MP4BoxCtab : public MP4Box {
    TT_PROPERTY_DEF_READONLY(uint32_t, colorTableSeed, 0);
    TT_PROPERTY_DEF_READONLY(uint16_t, colorTableFlags, 0x8000);
    TT_PROPERTY_DEF_READONLY(uint16_t, colorTableSize, 0);
    TT_PROPERTY_DEF_READONLY_NOINIT(std::vector<uint16_t>, colorArray);
};

class MP4BoxUdta : public MP4Box {
    
};

/**
 container for an individual track or stream
 moov
 mandatory
 */
class MP4BoxTrak : public MP4Box {
    
};

/**
 track header, overall information about the track
 trak
 mandatory
 */
class MP4BoxTkhd : public MP4Box {
    TT_PROPERTY_DEF_READONLY(uint64_t, creationTime, 0);
    TT_PROPERTY_DEF_READONLY(uint64_t, modificationTime, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, trackId, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, reserved, 0);
    TT_PROPERTY_DEF_READONLY(uint64_t, duration, 0);
    
    TT_PROPERTY_DEF_READONLY(uint64_t, reserved64, 0);
    TT_PROPERTY_DEF_READONLY(int16_t, layer, 0);
    TT_PROPERTY_DEF_READONLY(int16_t, alternateGroup, 0);
    TT_PROPERTY_DEF_READONLY(int16_t, volumn, 0);
    
    TT_PROPERTY_DEF_READONLY(uint16_t, reserved16, 0);
    
private:
    // provides a transformation matrix for the video; (u,v,w) are restricted here to (0,0,1), hex (0,0,0x40000000).
    int32_t matrix[9] = {0};
    
    TT_PROPERTY_DEF_READONLY(int32_t, width, 0);
    TT_PROPERTY_DEF_READONLY(int32_t, height, 0);
    
    
};

/**
 container for the media information in a track
 trak
 mandatory
 */
class MP4BoxMdia : public MP4Box {
    
};

/**
 media header, overall information about the media
 mdia
 mandatory
 */
class MP4BoxMdhd : public MP4FullBox {
    TT_PROPERTY_DEF_READONLY(uint64_t, creationTime, 0);
    TT_PROPERTY_DEF_READONLY(uint64_t, modification, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, timeScale, 0);
    TT_PROPERTY_DEF_READONLY(uint64_t, duration , 0);
    
    // bit(1) pad=0;
    TT_PROPERTY_DEF_READONLY(uint16_t, language, 0);
    TT_PROPERTY_DEF_READONLY(uint16_t, preDefined, 0);
};

/**
 handler, declares the media (handler) type
 mdia
 handler, declares the metadata (handler) type
 meta
 mandatory
 */
class MP4BoxHdlr : public MP4FullBox {
    TT_PROPERTY_DEF_READONLY(uint32_t, preDefined, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, handlerType, 0);
    
private:
    int32_t reserved[3] = {0};
    
    TT_PROPERTY_DEF_READONLY(std::string, name, nullptr);
};

/**
 media information container
 mdia
 mandatory
 */
class MP4BoxMinf : public MP4Box {
    
};

/**
 video media header, overall information (video track only)
 mdia
 not mandatory
 */
class MP4BoxVmhd : public MP4FullBox {
    TT_PROPERTY_DEF_READONLY(uint16_t, graphicsmode, 0);
private:
    // is a set of 3 colour values (red, green, blue) available for use by graphics modes
    uint16_t opcolor[3] = {0};
};

/**
 sound media header, overall information (sound track only)
 mdia
 not mandatory
 */
class MP4BoxSmhd : public MP4FullBox {
    TT_PROPERTY_DEF_READONLY(int16_t, balance, 0);
    TT_PROPERTY_DEF_READONLY(uint16_t, reserved, 0);
};

/**
 hint media header, overall information (hint track only)
 mdia
 not mandatory
 */
class MP4BoxHmhd : public MP4FullBox {
    TT_PROPERTY_DEF_READONLY(uint16_t, maxPDUsize, 0);
    TT_PROPERTY_DEF_READONLY(uint16_t, avgPDUsize, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, maxbitrate, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, avgbitrate, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, reserved, 0);
};

/**
 Null media header, overall information (some tracks only)
 mdia
 not mandatory
 */
class MP4BoxNmhd : public MP4FullBox {
    
};

/**
 data information box, container
 minf
 mandatory
 */
class MP4BoxDinf : public MP4Box {
    
};

/**
 data reference box, declares source(s) of media data in track
 The data reference object contains a table of data references (normally URLs) that declare the location(s) of the media data used within the presentation.
 dinf
 mandatory
 */
class MP4BoxDref : public MP4Box {
    // is an integer that counts the actual entries
    TT_PROPERTY_DEF_READONLY(uint32_t, entryCount, 0);
};

/**
 sample table box, container for the time/space map
 The sample table contains all the time and data indexing of the media samples in a track. Using the tables here, it is possible to locate samples in time, determine their type (e.g. I-frame or not), and determine their size, container, and offset into that container.

 the following sub-boxes are required: Sample Description, Sample Size, Sample To Chunk, and Chunk Offset. Further, the Sample Description Box shall contain at least one entry. A Sample Description Box is required because it contains the data reference index field which indicates which Data Reference Box to use to retrieve the media samples. Without the Sample Description, it is not possible to determine where the media samples are stored. The Sync Sample Box is optional. If the Sync Sample Box is not present, all samples are sync samples.
 Annex A provides a narrative description of random access using the structures defined in the Sample Table Box.
 
 minf
 mandatory
 */
class MP4BoxStbl : public MP4Box {
};

/**
 sample descriptions (codec types, initialization etc.)
 For video tracks, a VisualSampleEntry is used; for audio tracks, an AudioSampleEntry. Hint tracks use an entry format specific to their protocol, with an appropriate name.
 stbl
 mandatory
 */
class MP4BoxStsd : public MP4FullBox {
    // is an integer that gives the number of entries in the following table
    TT_PROPERTY_DEF_READONLY(uint32_t, entryCount, 0);
};

class MP4SttsEntry {
    // is an integer that counts the number of consecutive samples that have the given duration.
    TT_PROPERTY_DEF_READONLY(uint32_t, sampleCount, 0);
    // is an integer that gives the delta of these samples in the time-scale of the media.
    TT_PROPERTY_DEF_READONLY(uint32_t, sampleDelta, 0);
};

/**
 (decoding) time-to-sample
 stbl
 mandatory
 */
class MP4BoxStts : public MP4FullBox {
    // is an integer that gives the number of entries in the following table
    TT_PROPERTY_DEF_READONLY(uint32_t, entryCount, 0);
    TT_PROPERTY_DEF_READONLY_NOINIT(std::vector<MP4SttsEntry>, entrys);
};

class MP4CttsEntry {
    // is an integer that counts the number of consecutive samples that have the given offset.
    TT_PROPERTY_DEF_READONLY(uint32_t, sampleCount, 0);
    // is a non-negative integer that gives the offset between CT and DT, such that CT(n) = DT(n) + CTTS(n).
    TT_PROPERTY_DEF_READONLY(uint32_t, sampleOffset, 0);
};

/**
 (composition) time to sample
 stbl
 not mandatory
 */
class MP4BoxCtts : public MP4FullBox {
    // is an integer that gives the number of entries in the following table
    TT_PROPERTY_DEF_READONLY(uint32_t, entryCount, 0);
    TT_PROPERTY_DEF_READONLY_NOINIT(std::vector<MP4CttsEntry>, entrys);
};

class MP4StscEntry {
    TT_PROPERTY_DEF_READONLY(uint32_t, firstChunk, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, samplesPerChunk, 0);
    TT_PROPERTY_DEF_READONLY(uint32_t, sampleDescriptionIndex, 0);
};

/**
 sample-to-chunk, partial data-offset information
 Samples within the media data are grouped into chunks. Chunks can be of different sizes, and the samples within a chunk can have different sizes. This table can be used to find the chunk that contains a sample, its position, and the associated sample description.
 stbl
 mandatory
 */
class MP4BoxStsc : public MP4FullBox {
    // is an integer that gives the number of entries in the following table
    TT_PROPERTY_DEF_READONLY(uint32_t, entryCount, 0);
    TT_PROPERTY_DEF_READONLY_NOINIT(std::vector<MP4StscEntry>, entrys);
};

class MP4StcoEntry {
    TT_PROPERTY_DEF_READONLY(uint32_t, chunkOffset, 0);
};

/**
 chunk offset, partial data-offset information
 
 The chunk offset table gives the index of each chunk into the containing file. There are two variants, permitting the use of 32-bit or 64-bit offsets. The latter is useful when managing very large presentations. At most one of these variants will occur in any single instance of a sample table.
 
 Offsets are file offsets, not the offset into any box within the file
 
 stbl
 mandatory
 */
class MP4BoxStco : public MP4Box {
    // is an integer that gives the number of entries in the following table
    TT_PROPERTY_DEF_READONLY(uint32_t, entryCount, 0);
    TT_PROPERTY_DEF_READONLY_NOINIT(std::vector<MP4StcoEntry>, entrys);
};

/**
 movie extends box
 moov
 not mandatory
 */
class MP4BoxMvex : public MP4Box {
    
};

/**
 track extends defaults
 mvex
 mandatory
 */
class MP4BoxTrex : public MP4Box {
    
};

/**
 movie fragment
 not mandatory
 */
class MP4BoxMoof : public MP4Box {
    
};

/**
 track fragment
 moof
 not mandatory
 */
class MP4BoxTraf : public MP4Box {
    
};

/**
 track fragment header
 traf
 mandatory
 */
class MP4BoxTfhd : public MP4Box {
    
};

/**
 movie fragment random access
 not mandatory
 */
class MP4BoxMfra : public MP4Box {
    
};

/**
 movie fragment random access offset
 mfra
 mandatory
 */
class MP4BoxMfro : public MP4Box {
    
};

/**
 metadata
 not mandatory
 */
class MP4BoxMeta : public MP4Box {
    
};


/**
 media data container
 not mandatory
 */
class MP4BoxMdat : public MP4Box {
    
};

}

#endif /* TTMP4Box_hpp */
