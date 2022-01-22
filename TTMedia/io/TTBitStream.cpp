//
//  TTBitStream.cpp
//  TTMedia
//
//  Created by liang on 11/2/2021.
//  Copyright © 2021 tina. All rights reserved.
//

#include "TTBitStream.hpp"

using namespace TT;

BitStream::BitStream(TT_SP(IO) io) : _io(io)
{
    
}

BitStream::~BitStream()
{
    
}

bool BitStream::isEof()
{
    if (_io) {
        return _io->isEof();
    }
    return true;
}

bool BitStream::skipSize(size_t size)
{
    if (_io) {
        return _io->seek(size, kSeekCur);
    }
    return false;
}

int8_t BitStream::readInt8()
{
    int32_t n = readInt32ForBits(8);
    return n;
}

int16_t BitStream::readInt16()
{
    int32_t n = readInt32ForBits(16);
    return n;
}

int32_t BitStream::readInt32()
{
    int32_t n = readInt32ForBits(32);
    return n;
}

int64_t BitStream::readInt64()
{
    int64_t n = readInt64ForBits(64);
    return n;
}

uint8_t BitStream::readUInt8()
{
    uint32_t n = static_cast<uint32_t>(readInt32ForBits(8));
    return n;
}

uint16_t BitStream::readUInt16()
{
    uint32_t n = static_cast<uint32_t>(readInt32ForBits(16));
    return n;
}

uint32_t BitStream::readUInt24()
{
    uint32_t n = static_cast<uint32_t>(readInt32ForBits(24));
    return n;
}

uint32_t BitStream::readUInt32()
{
    uint32_t n = static_cast<uint32_t>(readInt64ForBits(32));
    return n;
}

uint64_t BitStream::readUInt64()
{
    uint64_t n = static_cast<uint64_t>(readInt64ForBits(64));
    return n;
}

uint32_t BitStream::readUInt32Le()
{
    uint32_t result = 0;
    if (_nBitsOfByte == 8) {
        uint32_t one = readUInt8();
        uint32_t two = readUInt8();
        uint32_t three = readUInt8();
        uint32_t four = readUInt8();
        result = one + (two << 8) + (three << 16) + (four << 24);
    }
    return result;
}

uint16_t BitStream::readUInt16Le()
{
    uint16_t result = 0;
    if (_nBitsOfByte == 8) {
        uint16_t one = readUInt8();
        uint16_t two = readUInt8();
        result = one + (two << 8);
    }
    return result;
}

float BitStream::readFloat()
{
    return 0;
}

double BitStream::readDouble()
{
    return 0;
}

size_t BitStream::readData(uint8_t *pBuf, size_t size)
{
    if (_io) {
        return _io->read(pBuf, size);
    }
    return 0;
}

int32_t BitStream::readInt32ForBits(uint32_t nBits)
{
    int32_t result = 0;
    while (nBits > 0) {
        if (_nBitsOfByte == 0) {
            _io->read(&_currentByte, 1);
            _nBitsOfByte = 8;
        }
        
        int8_t num = nBits;
        if (num > _nBitsOfByte) {
            num = _nBitsOfByte;
            _nBitsOfByte = 0;
        } else {
            _nBitsOfByte -= num;
        }
        
        result = result << num;
        result += _currentByte >> (8 - num);
        nBits -= num;
    }
    return result;
}

int64_t BitStream::readInt64ForBits(uint32_t nBits)
{
    int64_t result = 0;
    while (nBits > 0) {
        if (_nBitsOfByte == 0) {
            _io->read(&_currentByte, 1);
            _nBitsOfByte = 8;
        }
        
        int8_t num = nBits;
        if (nBits > _nBitsOfByte) {
            num = _nBitsOfByte;
        }
        result = result << num;
        result += _currentByte >> (8 - num);
        nBits -= num;
        _nBitsOfByte -= num;
    }
    return result;
}
