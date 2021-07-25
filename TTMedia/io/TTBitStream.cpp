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

uint32_t BitStream::readInt32(uint32_t nBits)
{
    uint32_t result = 0;
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

uint64_t BitStream::readInt64(uint32_t nBits)
{
    uint64_t result = 0;
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

uint8_t BitStream::readUInt8()
{
    int32_t n = readInt32(8);
    return n;
}

uint16_t BitStream::readUInt16()
{
    int32_t n = readInt32(16);
    return n;
}

uint32_t BitStream::readUInt24()
{
    int32_t n = readInt32(24);
    return n;
}

uint32_t BitStream::readUInt32()
{
    int32_t n = readInt32(32);
    return n;
}

uint64_t BitStream::readUInt64()
{
    int64_t n = readInt64(64);
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
