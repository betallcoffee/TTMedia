//
//  TTMP4Demuxer.cpp
//  TTMedia
//
//  Created by liang on 4/5/2021.
//  Copyright © 2021 tina. All rights reserved.
//


#include "TTMP4Demuxer.hpp"

using namespace TT;

MP4Demuxer::MP4Demuxer()
{
    
}

MP4Demuxer::~MP4Demuxer()
{

}

int MP4Demuxer::probe(std::shared_ptr<URL> url)
{
    const std::string &ext = url->extension();
    if ("mp4" == ext) {
        return 100;
    }
    return 0;
}

bool MP4Demuxer::open(std::shared_ptr<URL> url)
{
    _io = IO::createIO(url);
    if (_io) {
        _url = url;
        if (_io->open(_url, 0, 0)) {
            _bitStream = TT_MK_SP(BitStream)(_io);
            _parser = TT_MK_SP(MP4Parser)(_bitStream);
            do {
                if (!_parser->parseBox()) {
                    _io->close();
                    return false;
                }
            } while (_parser->moov() == nullptr);
            
            return true;
        }
    }
    return false;
}

void MP4Demuxer::close()
{
    
}

TT_SP(Packet) MP4Demuxer::read()
{
    return nullptr;
}

bool MP4Demuxer::seek(uint64_t pos)
{
    return false;
}

bool MP4Demuxer::isEOF()
{
    return false;
}


