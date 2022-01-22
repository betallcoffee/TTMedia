//
//  TTMP4BoxFactory.cpp
//  TTMedia
//
//  Created by liang on 16/1/2022.
//  Copyright © 2022 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTMP4BoxFactory.hpp"
#include "TTMP4Private.h"

using namespace TT;

TT_SP(MP4Box) MP4BoxFactory::createBox(TT_SP(BitStream) bitStream)
{
    if (bitStream == nullptr || bitStream->isEof()) {
        return nullptr;
    }
    
    char typeFourcc[5];
    char uuid[16];
    
    uint64_t headSize = 8;
    uint64_t size = bitStream->readUInt32();
    uint32_t type = bitStream->readUInt32();
    if (size == 1) {
        headSize = 16;
        size = bitStream->readUInt64();
    }
    
    MP4Box::FourcToStr(type, typeFourcc, sizeof(typeFourcc));
    LOG(INFO) << "type: " << typeFourcc << " size: " << size ;
    
    TT_SP(MP4Box) box = MP4BoxFactory::createBox(size, type);
    
    if (box) {
        box->setheadSize(headSize);
        if (box->isContainer() || box->parseData(bitStream) > 0) {
            return box;
        } else {
            box = nullptr;
        }
    }
    
    if (box == nullptr) {
        bool result = bitStream->skipSize(size - headSize);
        LOG(INFO) << "error skip box result: " << result;
    }
    
    return box;
}

TT_SP(MP4Box) MP4BoxFactory::createBox(uint64_t size, uint32_t type)
{
    switch (type) {
        case MP4_BOX_TYPE_ftyp:
        {
            TT_SP(MP4Box) ftyp = TT_MK_SP(MP4BoxFtyp)(size, type);
            return ftyp;
        }
            break;
        case MP4_BOX_TYPE_moov:
        {
            TT_SP(MP4Box) moov = TT_MK_SP(MP4BoxMoov)(size, type);
            return moov;
        }
            break;
        case MP4_BOX_TYPE_mvhd:
        {
            TT_SP(MP4Box) mvhd = TT_MK_SP(MP4BoxMvhd)(size, type);
            return mvhd;
        }
            break;
        case MP4_BOX_TYPE_trak:
        {
            TT_SP(MP4Box) trak = TT_MK_SP(MP4BoxTrak)(size, type);
            return trak;
        }
            break;
        case MP4_BOX_TYPE_tkhd:
        {
            TT_SP(MP4Box) tkhd = TT_MK_SP(MP4BoxTkhd)(size, type);
            return tkhd;
        }
            break;
        case MP4_BOX_TYPE_edts:
        {
            TT_SP(MP4Box) edts = TT_MK_SP(MP4BoxEdts)(size, type);
            return edts;
        }
            break;
        case MP4_BOX_TYPE_mdia:
        {
            TT_SP(MP4Box) mdia = TT_MK_SP(MP4BoxMdia)(size, type);
            return mdia;
        }
            break;
        case MP4_BOX_TYPE_mdhd:
        {
            TT_SP(MP4Box) mdhd = TT_MK_SP(MP4BoxMdhd)(size, type);
            return mdhd;
        }
            break;
        case MP4_BOX_TYPE_minf:
        {
            TT_SP(MP4Box) minf = TT_MK_SP(MP4BoxMinf)(size, type);
            return minf;
        }
            break;
        case MP4_BOX_TYPE_dinf:
        {
            TT_SP(MP4Box) dinf = TT_MK_SP(MP4BoxDinf)(size, type);
            return dinf;
        }
            break;
        case MP4_BOX_TYPE_hdlr:
        {
            TT_SP(MP4Box) hdlr = TT_MK_SP(MP4BoxHdlr)(size, type);
            return hdlr;
        }
        case MP4_BOX_TYPE_stbl:
        {
            TT_SP(MP4Box) stbl = TT_MK_SP(MP4BoxStbl)(size, type);
            return stbl;
        }
            break;
        case MP4_BOX_TYPE_stsd:
        {
            TT_SP(MP4Box) stsd = TT_MK_SP(MP4BoxStsd)(size, type);
            return stsd;
        }
            break;
        case MP4_BOX_TYPE_avc1:
        {
            TT_SP(MP4Box) avc1 = TT_MK_SP(MP4BoxAvc1)(size, type);
            return avc1;
        }
            break;
        case MP4_BOX_TYPE_avcC:
        {
            TT_SP(MP4Box) avcc = TT_MK_SP(MP4BoxAvcC)(size, type);
            return avcc;
        }
            break;
        default:
        {
            TT_SP(MP4Box) box = TT_MK_SP(MP4Box)(size, type);
            return box;
        }
            break;
    }
    return nullptr;
}
