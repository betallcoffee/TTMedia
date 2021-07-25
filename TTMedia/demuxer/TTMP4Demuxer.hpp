//
//  TTMP4Demuxer.hpp
//  TTMedia
//
//  Created by liang on 4/5/2021.
//  Copyright © 2021 tina. All rights reserved.
//

#ifndef TTMP4Demuxer_hpp
#define TTMP4Demuxer_hpp

#include "TTDef.h"
#include "TTURL.hpp"

#include "TTDemuxer.hpp"
#include "TTIO.hpp"
#include "TTBitStream.hpp"
#include "TTMP4Parser.hpp"

namespace TT {

class MP4Demuxer : public Demuxer {
public:
    MP4Demuxer();
    ~MP4Demuxer();
    
    static int probe(std::shared_ptr<URL> url);
    
    bool open(std::shared_ptr<URL> url) override;
    void close() override;
    
    TT_SP(Packet) read() override;
    
    bool seek(uint64_t pos) override;
    bool isEOF() override;
    
    TT_PROPERTY_DEF_READONLY(TT_SP(URL), url, nullptr)
    TT_PROPERTY_DEF_READONLY(TT_SP(IO), io, nullptr)
    TT_PROPERTY_DEF_READONLY(TT_SP(BitStream), bitStream, nullptr)
    TT_PROPERTY_DEF_READONLY(TT_SP(MP4Parser), parser, nullptr)
};

}

#endif /* TTMP4Demuxer_hpp */
