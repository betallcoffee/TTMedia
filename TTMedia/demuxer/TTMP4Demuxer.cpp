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
    
    close();
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
            _bitStream = mp4_bs_create(nullptr, _io->size(), MP4_BITS_READ_CUSTOM);
            if (_bitStream == nullptr) {
                return false;
            }
            
            mp4_bs_set_read_opaque(_bitStream, this);
            mp4_bs_set_read_byte_func(_bitStream, MP4Demuxer::readByte);
            mp4_bs_set_read_data_func(_bitStream, MP4Demuxer::readData);
            mp4_bs_set_seek_func(_bitStream, MP4Demuxer::seekOffset);
            mp4_bs_set_read_available_func(_bitStream, MP4Demuxer::readAvailable);
            do {
                if (!parseBox()) {
                    _io->close();
                    return false;
                }
            } while (_moov == nullptr || _mdat == nullptr);
            
            return true;
        }
    }
    
    return false;
}

void MP4Demuxer::close() {
    if (_bitStream) {
        mp4_bs_destroy(_bitStream);
        _bitStream = nullptr;
    }
    
    if (_io) {
        _io->close();
    }
}

std::shared_ptr<Packet> MP4Demuxer::read() {
    if (!_isOpened) {
        return nullptr;
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

uint8_t MP4Demuxer::readByte(mp4_bits_t *bs) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        MP4Demuxer *self = (MP4Demuxer *)opaque;
        return self->readByte();
    }
    return 0;
}

uint8_t MP4Demuxer::readByte() {
    uint8_t byte = 0;
    _io->read(&byte, 1);
    return byte;
}

uint32_t MP4Demuxer::readData(mp4_bits_t *bs, char *data, uint32_t nbBytes) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        MP4Demuxer *self = (MP4Demuxer *)opaque;
        return self->readData(data, nbBytes);
    }
    return 0;
}

uint32_t MP4Demuxer::readData(char *data, uint32_t nbBytes) {
    size_t readSize = _io->read((uint8_t *)data, nbBytes);
    if (readSize < nbBytes) {
        return 0;
    }
    return nbBytes;
}

int MP4Demuxer::seekOffset(mp4_bits_t *bs, uint64_t offset) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        MP4Demuxer *self = (MP4Demuxer *)opaque;
        return self->seekOffset(offset);
    }
    return -1;
}

int MP4Demuxer::seekOffset(uint64_t offset) {
    if (_io->seek(offset)) {
        return 0;
    }
    return -1;
}

uint64_t MP4Demuxer::readAvailable(mp4_bits_t *bs) {
    void *opaque = mp4_bs_get_read_opaque(bs);
    if (opaque) {
        MP4Demuxer *self = (MP4Demuxer *)opaque;
        return self->readAvailable();
    }
    return 0;
}

uint64_t MP4Demuxer::readAvailable() {
    return _io->readAvailable();
}

bool MP4Demuxer::parseBox() {
    size_t readSize = _io->readAvailable();
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

bool MP4Demuxer::parseBoxBody() {
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
