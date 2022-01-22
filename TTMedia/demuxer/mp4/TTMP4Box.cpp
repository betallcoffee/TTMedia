//
//  TTMP4Box.cpp
//  TTMediaExample
//
//  Created by liang on 20/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include <vector>

#include "easylogging++.h"

#include "TTMP4Box.hpp"
#include "TTMP4BoxFactory.hpp"

using namespace TT;

MP4Box::MP4Box(uint64_t size, uint32_t type, bool isContainer)
: _size(size)
, _type(type)
, _isContainer(isContainer)
{
}

MP4Box::~MP4Box()
{
    
}

bool MP4Box::FourcToStr(uint32_t type, char *buf, size_t size)
{
    uint32_t i;
    int ch;
    
    if (buf == NULL || size < 5)
        return false;
    
    for (i = 0; i < 4; i++, buf++) {
        ch = type >> (8 * (3 - i)) & 0xff;
        if (isprint(ch))
            *buf = ch;
        else
            *buf = '.';
    }
    
    *buf = 0;
    
    return true;
}

uint64_t MP4Box::parseData(TT_SP(BitStream) bitStream)
{
    if (!this->isContainer()) {
        _data = TT_MK_SP(std::vector<uint8_t>)();
        _data->resize(this->dataSize());
        bitStream->readData(_data->data(), this->dataSize());
        return this->dataSize();
    } else {
        return 0;
    }
}

void MP4Box::addChildBox(TT_SP(MP4Box) box)
{
    _childBoxsSize += box->size();
    this->childBoxs.push_back(box);
}

bool MP4Box::hasAllChildBoxs()
{
    if (this->childBoxsSize() >= this->dataSize()) {
        return true;
    }
    return false;
}

uint64_t MP4Box::dataSize()
{
    return this->size() - this->headSize();
}

#pragma mark -- full box
uint64_t MP4FullBox::parseData(TT_SP(BitStream) bitStream)
{
    this->setversion(bitStream->readUInt8());
    this->setflags(bitStream->readUInt24());
    return 4;
}

#pragma mark -- Box Ftyp
uint64_t MP4BoxFtyp::parseData(TT_SP(BitStream) bitStream)
{
    _majorBrand = bitStream->readUInt32();
    char fourcc[5];
    MP4Box::FourcToStr(_majorBrand, fourcc, sizeof(fourcc));
    LOG(INFO) << "majorBrand: " << fourcc;
    _minorVersion = bitStream->readUInt32();
    
    uint64_t remainSize = this->size() - headSize() - 8;
    _compatibleBrandsCount = remainSize / 4;
    _compatibleBrands = new uint32_t[_compatibleBrandsCount]();
    for (int i = 0; i < _compatibleBrandsCount; i++) {
        _compatibleBrands[i] = bitStream->readUInt32();
        MP4Box::FourcToStr(_compatibleBrands[i], fourcc, sizeof(fourcc));
        LOG(INFO) << "compatibleBrands: " << fourcc;
    }
    
    return this->dataSize();
}

#pragma mark -- Box Mvhd
uint64_t MP4BoxMvhd::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4FullBox::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    if (this->version() == 1) {
        _creationTime = bitStream->readUInt64();
        _modificationTime = bitStream->readUInt64();
        _timeScale = bitStream->readUInt32();
        _duration = bitStream->readUInt64();
    } else {
        _creationTime = bitStream->readUInt32();
        _modificationTime = bitStream->readUInt32();
        _timeScale = bitStream->readUInt32();
        _duration = bitStream->readUInt32();
    }
    
    _rate = bitStream->readUInt32();
    _volume = bitStream->readUInt16();
    bitStream->skipSize(2); // const bit(16) reserved = 0
    bitStream->skipSize(8); // const unsigned int(32)[2] reserved = 0;
    bitStream->skipSize(36); // template int(32)[9] matrix = { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 }; Unity matrix
    bitStream->skipSize(24); // bit(32)[6]  pre_defined = 0;
    _nextId = bitStream->readUInt32();
    
    return this->dataSize();
}

#pragma mark -- Box Tkhd
/**
 The default value of the track header flags for media tracks is 7 (track_enabled, track_in_movie, track_in_preview). If in a presentation all tracks have neither track_in_movie nor track_in_preview set, then all tracks shall be treated as if both flags were set on all tracks. Hint tracks should have the track header flags set to 0, so that they are ignored for local playback and preview.
 width and height specify the track's visual presentation size as fixed-point 16.16 values. These need not be the same as the pixel dimensions of the images, which is documented in the sample description(s); all images in the sequence are scaled to this size, before any overall transformation of the track represented by the matrix. The pixel dimensions of the images are the default values.
 */
uint64_t MP4BoxTkhd::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4FullBox::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    if (this->version() == 1) {
        _creationTime = bitStream->readUInt64();
        _modificationTime = bitStream->readUInt64();
        _trackId = bitStream->readUInt32();
        bitStream->skipSize(4); // const unsigned int(32)  reserved = 0;
        _duration = bitStream->readUInt64();
    } else {
        _creationTime = bitStream->readUInt32();
        _modificationTime = bitStream->readUInt32();
        _trackId = bitStream->readUInt32();
        bitStream->skipSize(4); // const unsigned int(32)  reserved = 0;
        _duration = bitStream->readUInt32();
    }
    
    bitStream->skipSize(8); // const unsigned int(32)[2] reserved = 0;
    _layer = bitStream->readInt16();
    _alternateGroup = bitStream->readInt16();
    _volumn = bitStream->readInt16();
    bitStream->skipSize(2); // const unsigned int(16) reserved = 0;
    bitStream->skipSize(36); // template int(32)[9] matrix = { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 }; Unity matrix
    _width = bitStream->readUInt32();
    _height = bitStream->readUInt32();
    
    return this->dataSize();
}

#pragma mark -- Box Mdhd
uint64_t MP4BoxMdhd::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4FullBox::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    if (this->version() == 1) {
        _creationTime = bitStream->readUInt64();
        _modificationTime = bitStream->readUInt64();
        _timeScale = bitStream->readUInt32();
        _duration = bitStream->readUInt64();
    } else {
        _creationTime = bitStream->readUInt32();
        _modificationTime = bitStream->readUInt32();
        _timeScale = bitStream->readUInt32();
        _duration = bitStream->readUInt32();
    }
    
    bitStream->skipSize(2);
    _preDefined = bitStream->readInt16();
    
    return this->dataSize();
}

#pragma mark -- Box Hdlr
uint64_t MP4BoxHdlr::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4FullBox::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    _preDefined = bitStream->readUInt32();
    _handlerType = bitStream->readUInt32();
    char fourcc[5];
    MP4Box::FourcToStr(_handlerType, fourcc, sizeof(fourcc));
    LOG(INFO) << "hdlr handlerType: " << fourcc;
    
    bitStream->skipSize(12); // const unsigned int(32)[3] reserved = 0;
    
    size_t nameSize = this->dataSize() - result - 20;
    std::vector<uint8_t> buffer(nameSize);
    bitStream->readData(buffer.data(), nameSize);
    _name.assign(buffer.begin(), buffer.end());
    LOG(INFO) << "hdlr name: " << _name;
    return this->dataSize();
}

#pragma mark -- Box Stsd
uint64_t MP4BoxStsd::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4FullBox::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    _entryCount = bitStream->readUInt32();
    
    for (int i = 0; i < _entryCount; i++) {
        TT_SP(MP4Box) childBox = MP4BoxFactory::createBox(bitStream);
    }
    

    return this->dataSize();
}

uint64_t MP4BoxSampleEntry::parseData(TT_SP(BitStream) bitStream)
{
    bitStream->skipSize(6); // const unsigned int(8)[6] reserved = 0;
    _dataReferenceIndex = bitStream->readUInt16();
    return 8;
}
                                      
uint64_t MP4BoxHintSampleEntry::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4BoxSampleEntry::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    bitStream->skipSize(this->dataSize() - result);
    
    return this->dataSize();
}

uint64_t MP4BoxVisualSampleEntry::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4BoxSampleEntry::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    _preDefined1 = bitStream->readUInt16();
    _reserved1 = bitStream->readUInt16();
    bitStream->skipSize(12); // unsigned int(32)[3] pre_defined = 0;
    _width = bitStream->readUInt16();
    _height = bitStream->readUInt16();
    _horizresolution = bitStream->readUInt32();
    _vertresolution = bitStream->readUInt32();
    _reserved2 = bitStream->readUInt32();
    _frameCount = bitStream->readUInt16();
    
    // It is formatted in a fixed 32-byte field, with the
    // first byte set to the number of bytes to be displayed, followed by that number of bytes of displayable
    // data, and then padding to complete 32 bytes total (including the size byte). The field may be set to 0
    uint8_t compressLen = bitStream->readUInt8();
    if (compressLen > 31) {
        compressLen = 31;
    }
    std::vector<uint8_t> buffer(compressLen);
    bitStream->readData(buffer.data(), compressLen);
    _compressorname.assign(buffer.begin(), buffer.end());
    bitStream->skipSize(31 - compressLen);
    
    _depth = bitStream->readUInt16();
    _preDefined3 = bitStream->readUInt16();
    
    return this->dataSize() - result - 12 - 2 * 7 - 4 * 4;
}

uint64_t MP4BoxAudioSampleEntry::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4BoxSampleEntry::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    bitStream->skipSize(8); // const unsigned int(32)[2] reserved = 0;
    _channelcount = bitStream->readUInt16();
    _samplesize = bitStream->readUInt16();
    _preDefined = bitStream->readUInt16();
    _reserved2 = bitStream->readUInt16();
    _samplerate = bitStream->readUInt16();
    
    return this->dataSize() - result - 2 * 5;
}

#pragma mark -- 14496-15
#pragma mark -- Box Avc1
uint64_t MP4BoxAvc1::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4BoxVisualSampleEntry::parseData(bitStream);
    if (result == 0) {
        return result;
    }
    
    TT_SP(MP4Box) childBox = MP4BoxFactory::createBox(bitStream);
    
    return this->dataSize();
}

#pragma mark -- Box AvcC
uint64_t MP4BoxAvcC::parseData(TT_SP(BitStream) bitStream)
{
    uint64_t result = MP4Box::parseData(bitStream);
    return result;
}
