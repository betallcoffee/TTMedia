//
//  TTLibMP4Demuxer.cpp
//  TTMedia
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTLibMP4Demuxer.hpp"
#include "TTIO.hpp"

using namespace TT;

LibMP4Demuxer::LibMP4Demuxer() : _url(nullptr)
, _mutex(PTHREAD_MUTEX_INITIALIZER)
, _isEOF(false)
, _bitStream(nullptr)
, _newBox(nullptr)
, _moov(nullptr)
, _mdat(nullptr)
, _brand(nullptr)
, _audioTrackIdx(-1)
, _audioNextSampleIdx(-1)
, _audioNextOffset(0)
, _videoTrackIdx(-1)
, _videoNextSampleIdx(-1)
, _videoNextOffset(0)
{
    _rootBoxes = mp4_list_create();
}

LibMP4Demuxer::~LibMP4Demuxer() {
    if (_rootBoxes) {
        mp4_list_destroy(_rootBoxes);
        _rootBoxes = nullptr;
    }
    
    close();
}

int LibMP4Demuxer::probe(std::shared_ptr<URL> url) {
    const std::string &ext = url->extension();
    if ("mp4" == ext) {
        return 100;
    }
    return 0;
}

bool LibMP4Demuxer::open(std::shared_ptr<URL> url) {
    _io = IO::createIO(url);
    if (_io) {
        _url = url;
        if (_io->open(_url, 0, 0)) {
            _bitStream = mp4_bs_create(nullptr, _io->size(), MP4_BITS_READ_CUSTOM);
            if (_bitStream == nullptr) {
                return false;
            }
            
            mp4_bs_set_read_opaque(_bitStream, this);
            mp4_bs_set_read_byte_func(_bitStream, LibMP4Demuxer::readByte);
            mp4_bs_set_read_data_func(_bitStream, LibMP4Demuxer::readData);
            mp4_bs_set_seek_func(_bitStream, LibMP4Demuxer::seekOffset);
            mp4_bs_set_read_available_func(_bitStream, LibMP4Demuxer::readAvailable);
            do {
                if (!parseBox()) {
                    _io->close();
                    return false;
                }
            } while (_moov == nullptr || _mdat == nullptr);
            
            dumpStreamInfo();
            _isOpened = true;
            
            return true;
        }
    }
    
    return false;
}

void LibMP4Demuxer::close() {
    if (_bitStream) {
        mp4_bs_destroy(_bitStream);
        _bitStream = nullptr;
    }
    
    if (_io) {
        _io->close();
    }
}

TT_SP(Packet) LibMP4Demuxer::read() {
    if (!_isOpened) {
        return nullptr;
    }
    
    std::shared_ptr<Packet> packet = nullptr;
    if (_videoTrackIdx >= 0 && _audioTrackIdx < 0) {
        packet = readVideo();
    } else if (_videoTrackIdx < 0 && _audioTrackIdx >= 0) {
        packet = readAudio();
    } else if (_videoTrackIdx >= 0 && _audioTrackIdx >= 0) {
        if (_videoNextOffset < _audioNextOffset) {
            packet = readVideo();
        } else if (_videoNextOffset > _audioNextOffset) {
            packet = readAudio();
        } else {
            LOG(ERROR) << "videoNextOffset is equal audioNextOffset: " << _audioNextOffset;
        }
    } else {
        LOG(ERROR) << "There is none track";
    }
    
    return packet;
}

std::shared_ptr<Packet> LibMP4Demuxer::readAudio() {
    if (_audioTrackIdx < 0) {
        return nullptr;
    }
    
    std::shared_ptr<Packet> packet = std::make_shared<Packet>();
    packet->type = kPacketTypeAudio;
    packet->pos = _audioNextOffset;
    packet->dts = _moov->get_sample_dts(_moov, _audioTrackIdx, _audioNextSampleIdx);
    packet->duration = _moov->get_sample_duration(_moov, _audioTrackIdx, _audioNextSampleIdx);
    struct mp4_stbl_box *audioStbl = _moov->get_track_of_idx(_moov, _audioTrackIdx);
    uint32_t sampleCts = 0;
    audioStbl->get_sample_cts_offset(audioStbl, _audioNextSampleIdx, &sampleCts);
    packet->pts = packet->dts + sampleCts;
    
    uint32_t sampleSize = _moov->get_sample_size(_moov, _audioTrackIdx, _audioNextSampleIdx);
    if (readPacketData(packet, sampleSize)) {
        _audioNextSampleIdx++;
        audioStbl->get_sample_file_offset(audioStbl, _audioNextSampleIdx, &_audioNextOffset);
        return packet;
    } else {
        return nullptr;
    }
}

std::shared_ptr<Packet> LibMP4Demuxer::readVideo() {
    if (_videoTrackIdx < 0) {
        return nullptr;
    }
    
    std::shared_ptr<Packet> packet = std::make_shared<Packet>();
    packet->type = kPacketTypeVideo;
    packet->pos = _videoNextOffset;
    packet->dts = _moov->get_sample_dts(_moov, _videoTrackIdx, _videoNextSampleIdx);
    packet->duration = _moov->get_sample_duration(_moov, _videoTrackIdx, _videoNextSampleIdx);
    struct mp4_stbl_box *videoStbl = _moov->get_track_of_idx(_moov, _videoTrackIdx);
    uint32_t sampleCts = 0;
    videoStbl->get_sample_cts_offset(videoStbl, _videoNextSampleIdx, &sampleCts);
    packet->pts = packet->dts + sampleCts;
    
    uint32_t sampleSize = _moov->get_sample_size(_moov, _videoTrackIdx, _videoNextSampleIdx);
    if (readPacketData(packet, sampleSize)) {
        _videoNextSampleIdx++;
        videoStbl->get_sample_file_offset(videoStbl, _videoNextSampleIdx, &_videoNextOffset);
        return packet;
    } else {
        return nullptr;
    }
}

bool LibMP4Demuxer::readPacketData(std::shared_ptr<Packet> packet, size_t size) {
    if (packet == nullptr || size <= 0) {
        return false;
    }
    
    packet->allocData(size);
    uint8_t *data = packet->data();
    size_t readSize = _io->read(data, size);
    if (readSize == size) {
        packet->setsize(size);
        return true;
    } else {
        return false;
    }
}

bool LibMP4Demuxer::seek(uint64_t pos) {
    return false;
}

uint8_t LibMP4Demuxer::readByte(mp4_bits_t *bs) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        LibMP4Demuxer *self = (LibMP4Demuxer *)opaque;
        return self->readByte();
    }
    return 0;
}

uint8_t LibMP4Demuxer::readByte() {
    uint8_t byte = 0;
    _io->read(&byte, 1);
    return byte;
}

uint32_t LibMP4Demuxer::readData(mp4_bits_t *bs, char *data, uint32_t nbBytes) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        LibMP4Demuxer *self = (LibMP4Demuxer *)opaque;
        return self->readData(data, nbBytes);
    }
    return 0;
}

uint32_t LibMP4Demuxer::readData(char *data, uint32_t nbBytes) {
    size_t readSize = _io->read((uint8_t *)data, nbBytes);
    if (readSize < nbBytes) {
        return 0;
    }
    return nbBytes;
}

int LibMP4Demuxer::seekOffset(mp4_bits_t *bs, uint64_t offset) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        LibMP4Demuxer *self = (LibMP4Demuxer *)opaque;
        return self->seekOffset(offset);
    }
    return -1;
}

int LibMP4Demuxer::seekOffset(uint64_t offset) {
    if (_io->seek(offset)) {
        return 0;
    }
    return -1;
}

uint64_t LibMP4Demuxer::readAvailable(mp4_bits_t *bs) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        LibMP4Demuxer *self = (LibMP4Demuxer *)opaque;
        return self->readAvailable();
    }
    return 0;
}

uint64_t LibMP4Demuxer::readAvailable() {
    return _io->readAvailable();
}

bool LibMP4Demuxer::parseBox() {
    int64_t readSize = _io->readAvailable();
    if (readSize < 8) {
        LOG(ERROR) << "can not read box header, so close";
        return false;
    }
    _newBoxSize = parseBoxHeader(&_newBox, _bitStream);
    if (_newBoxSize < 0 || _newBox == nullptr) {
        LOG(ERROR) << "parse box header failed, so close";
        return false;
    } else {
        switch (_newBox->type) {
                /*moov box */
            case MP4_BOX_TYPE_moov: {
                if (_moov) {
                    _newBox->destroy(_newBox);
                    _newBox = nullptr;
                    return false;
                } else {
                    if (parseBoxBody()) {
                        _moov = (struct mp4_moov_box *)_newBox;
                    }
                }
                /*set our pointer to the movie */
                // mov->moov->mov = mov;
            }
                break;
            case MP4_BOX_TYPE_ftyp: {
                /*one and only one ftyp box */
                if (_brand) {
                    _newBox->destroy(_newBox);
                    _newBox = nullptr;
                    return false;
                } else {
                    if (parseBoxBody()) {
                        _brand = (struct mp4_ftyp_box *)_newBox;
                    }
                }
            }
                break;
            case MP4_BOX_TYPE_mdat: {
                _mdat = (struct mp4_mdat_box *)_newBox;
            }
                break;
            default: {
                if (!parseBoxBody()) {
                    return false;
                }
            }
                break;
        }
        
        /* chain this box with the father and the other at same level */
        if (_newBox) {
            mp4_list_add(_rootBoxes, _newBox);
        }
        
        return true;
    }
}

int64_t LibMP4Demuxer::parseBoxHeader(struct mp4_box **box, mp4_bits_t * bs) {
    uint32_t type, hdr_size;
    uint64_t size, start;
    char type_fourcc[5];
    char uuid[16];
    struct mp4_box *new_box;
    
    if ((bs == NULL) || (box == NULL))
        return -1;
    
    *box = NULL;
    start = mp4_bs_get_position(bs);
    
    size = (uint64_t) mp4_bs_read_u32(bs);
    type = mp4_bs_read_u32(bs);
    hdr_size = 8;
    
    mp4_fourcc_to_str(type, type_fourcc, sizeof(type_fourcc));
    LOG(INFO) << "size: " << size << " type: " << type_fourcc;
    
    /* extend large size */
    if (size == 1) {
        size = mp4_bs_read_u64(bs);
        hdr_size += 8;
        LOG(INFO) << "size: " << size;
    } else if (size == 0) {
        /* box extend to the end of file */
        size = 8;
    }
    
    /* extended type */
    if (type == MP4_BOX_TYPE_uuid) {
        memset(uuid, 0, 16);
        mp4_bs_read_data(bs, uuid, 16);
        hdr_size += 16;
    }
    
    /* check size valid */
    if (size < hdr_size) {
        LOG(ERROR) << "box size: " << size << " box header size: " << hdr_size;
        return -1;
    }
    
    /* create the box based on the type */
    new_box = mp4_box_create(type);
    if (!new_box) {
        LOG(ERROR) << "box create failed";
        return -1;
    }
    
    if (!new_box->type)
        new_box->type = type;
    
    if (type == MP4_BOX_TYPE_uuid) {
        memcpy(new_box->uuid, uuid, 16);
    }
    
    /* box data size */
    new_box->size = size - hdr_size;
    *box = new_box;
    
    return size;
}

bool LibMP4Demuxer::parseBoxBody() {
    int ret = _newBox->read(_newBox, _bitStream);
    if (ret) {
        LOG(ERROR) << "read failed";
        _newBox->destroy(_newBox);
        _newBox = nullptr;
        return false;
    }
    _newBox->size = _newBoxSize;
    return true;
}

void LibMP4Demuxer::dumpStreamInfo() {
    if (nullptr == _moov) {
        return;
    }
    
    uint32_t trackCount = _moov->get_nr_of_tracks(_moov);
    LOG(INFO) << "track count: " << trackCount;
    for (int i = 0; i < trackCount; i++) {
        std::shared_ptr<Stream> stream = std::make_shared<Stream>();
        
        uint32_t trackId = _moov->get_track_id(_moov, i);
        uint64_t trackDuration = _moov->get_track_duration(_moov, trackId);
        
        uint32_t mediaType = _moov->get_media_type(_moov, trackId);
        uint64_t mediaDuration = _moov->get_media_duration(_moov, trackId);
        uint32_t mediaTimescale = _moov->get_media_timescale(_moov, trackId);
        
        uint32_t sampleCount = _moov->get_nr_of_samples(_moov, trackId);
        
        stream->setindex(i);
        stream->settimeScale(TTRational{1, mediaTimescale});
        stream->setduration(mediaDuration);
        
        LOG(INFO) << "trackId: " << trackId << " trankDuration: " << trackDuration;
        char typeFourcc[5];
        mp4_fourcc_to_str(mediaType, typeFourcc, sizeof(typeFourcc));
        LOG(INFO) << "mediaType: " << typeFourcc;
        LOG(INFO) << "mediaDuration: " << mediaDuration << " mediaTimescale: " << mediaTimescale;
        
        LOG(INFO) << "sampleCount: " << sampleCount;
        
        switch (mediaType) {
            case MP4_FOURCC('v', 'i', 'd', 'e'): {
                _videoTrackIdx = i;
                _videoTrackId = trackId;
                stream->settype(kStreamTypeVideo);
                _videoStream = stream;
                
                dumpStreamVideoInfo();
            }
                break;
            case MP4_FOURCC('s', 'o', 'u', 'n'): {
                _audioTrackIdx = i;
                _audioTrackId = trackId;
                stream->settype(kStreamTypeAudio);
                _audioStream = stream;
                
                dumpStreamAudioInfo();
            }
                break;
            default:
                break;
        }
    }
}

void LibMP4Demuxer::dumpStreamAudioInfo() {
    _audioNextSampleIdx = 0;
    _audioNextOffset = 0;
    struct mp4_stbl_box *audioStbl = _moov->get_track_of_idx(_moov, _audioTrackIdx);
    audioStbl->get_sample_file_offset(audioStbl, _audioNextSampleIdx, &_audioNextOffset);
    
    _audioCodecParams = std::make_shared<CodecParams>();
    _audioCodecParams->codecType = kMediaTypeAudio;
    uint32_t sampleRate = 0;
    uint32_t channels = 0;
    uint8_t bitsPerSample = 0;
    uint8_t version = 0;
    _moov->get_audio_info(_moov, _audioTrackId, &sampleRate, &channels, &bitsPerSample, &version);
    _audioCodecParams->sampleRate = sampleRate;
    _audioCodecParams->channels = channels;
    _audioCodecParams->bits_per_raw_sample = bitsPerSample;
}

void LibMP4Demuxer::dumpStreamVideoInfo() {
    _videoNextSampleIdx = 0;
    _videoNextOffset = 0;
    struct mp4_stbl_box *videoStbl = _moov->get_track_of_idx(_moov, _videoTrackIdx);
    videoStbl->get_sample_file_offset(videoStbl, _videoNextSampleIdx, &_videoNextOffset);
    
    _videoCodecParams = std::make_shared<CodecParams>();
    _videoCodecParams->codecType = kMediaTypeVideo;
    uint32_t width = 0, height = 0;
    _moov->get_visual_info(_moov, _videoTrackId, &width, &height);
    _videoCodecParams->width = width;
    _videoCodecParams->height = height;
    
    uint8_t seqHdrNum = 0;
    char **seqHdr = nullptr;
    uint16_t *seqHdrSize = nullptr;
    
    uint8_t picHdrNum = 0;
    char **picHdr = nullptr;
    uint16_t *picHdrSize = nullptr;
    _moov->get_h264_seq_pic_hdrs(_moov, _videoTrackId,
                                 &seqHdrNum, &seqHdr, &seqHdrSize,
                                 &picHdrNum, &picHdr, &picHdrSize);
    if (seqHdrNum <= 0 || picHdrNum <= 0) {
        LOG(ERROR) << "Did not find sps/pps";
    }
    
    size_t extraSize = seqHdrSize[0] + picHdrSize[0] + 8;
    _videoStream->allocExtraData(extraSize);
    uint8_t *extraData = _videoStream->extraData();
    if (extraData) {
        uint8_t startCode[4] = {0, 0, 0, 1};
        int offset = 0;
        memcpy(extraData + offset, startCode, 4);
        offset += 4;
        memcpy(extraData + offset, seqHdr[0], seqHdrSize[0]);
        offset += seqHdrSize[0];
        memcpy(extraData + offset, startCode, 4);
        offset += 4;
        memcpy(extraData + offset, picHdr[0], picHdrSize[0]);
    }
}
