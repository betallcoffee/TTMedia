//
//  TTMP4Demuxer.cpp
//  TTPlayerExample
//
//  Created by liang on 12/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTMP4Demuxer.hpp"
#include "TTIO.hpp"

using namespace TT;

MP4Demuxer::MP4Demuxer() : _url(nullptr)
, _mutex(PTHREAD_MUTEX_INITIALIZER)
, _isEOF(false)
, _bitStream(nullptr)
, _newBox(nullptr)
, _moov(nullptr)
, _mdat(nullptr)
, _brand(nullptr) {
    _rootBoxes = mp4_list_create();
}

MP4Demuxer::~MP4Demuxer() {
    if (_rootBoxes) {
        mp4_list_destroy(_rootBoxes);
        _rootBoxes = nullptr;
    }
}

int MP4Demuxer::probe(std::shared_ptr<URL> url) {
    const std::string &ext = url->extension();
    if ("mp4" == ext) {
        return 100;
    }
    return 0;
}

bool MP4Demuxer::open(std::shared_ptr<URL> url) {
    _io = IO::createIO(url);
    if (_io) {
        _url = url;
        if (_io->open(_url, 0, 0)) {
            do {
                if (!parseBox()) {
                    _io->close();
                    return false;
                }
            } while (_moov == nullptr);
            
            return true;
        }
    }
    
    return false;
}

void MP4Demuxer::close() {
    
}

std::shared_ptr<Packet> MP4Demuxer::read() {
    if (parseBox()){
        
    }
    return nullptr;
}

bool MP4Demuxer::seek(uint64_t pos) {
    return false;
}

std::shared_ptr<AudioCodec> MP4Demuxer::audioCodec() {
    return _audioCodec;
}

std::shared_ptr<VideoCodec> MP4Demuxer::videoCodec() {
    return _videoCodec;
}

bool MP4Demuxer::parseBox() {
    _newBox = nullptr;
    char boxHeader[8] = {0};
    size_t readSize = _io->read((uint8_t *)boxHeader, 8);
    if (readSize < 8) {
        LOG(ERROR) << "can not read box header, so close";
        return false;
    }
    struct mp4_bits *bs = mp4_bs_create(boxHeader, 8, MP4_BITS_READ);
    int64_t size = parseBoxHeader(&_newBox, bs);
    mp4_bs_destroy(bs);
    if (size < 0 || _newBox == nullptr) {
        LOG(ERROR) << "parse box header failed, so close";
        return false;
    }
    
    char *boxData = (char *)malloc(_newBox->size);
    readSize = _io->read((uint8_t *)boxData, _newBox->size);
    if (readSize < _newBox->size) {
        LOG(ERROR) << "can not read box data, so close";
        free(boxData);
        return false;
    }
    bs = mp4_bs_create(boxData, _newBox->size, MP4_BITS_READ);
    int ret = _newBox->read(_newBox, bs);
    mp4_bs_destroy(bs);
    free(boxData);
    if (ret) {
        LOG(ERROR) << "read failed";
        _newBox->destroy(_newBox);
        _newBox = nullptr;
        return false;
    }
    _newBox->size = size;

    switch (_newBox->type) {
            /*moov box */
        case MP4_BOX_TYPE_moov: {
            if (_moov) {
                _newBox->destroy(_newBox);
                _newBox = nullptr;
            } else {
                _moov = (struct mp4_moov_box *)_newBox;
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
            } else {
                _brand = (struct mp4_ftyp_box *)_newBox;
            }
        }
            break;
        default:
            break;
    }
    
    /* chain this box with the father and the other at same level */
    if (_newBox) {
        mp4_list_add(_rootBoxes, _newBox);
    }
    
    return true;
}

int64_t MP4Demuxer::parseBoxHeader(struct mp4_box **box, mp4_bits_t * bs) {
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
        size = mp4_bs_available(bs) + 8;
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
