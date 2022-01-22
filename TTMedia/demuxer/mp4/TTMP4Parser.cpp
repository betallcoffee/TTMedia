//
//  TTMP4Parser.cpp
//  TTMedia
//
//  Created by liang on 24/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTDef.h"

#include "TTMP4Parser.hpp"
#include "TTMP4Private.h"
#include "TTMP4BoxFactory.hpp"

using namespace TT;

MP4Parser::MP4Parser(TT_SP(BitStream) bitStream) :
 _bitStream(bitStream)
{

}

MP4Parser::~MP4Parser()
{
    
}

bool MP4Parser::parse()
{
    if (_bitStream == nullptr || _bitStream->isEof()) {
        this->setisEof(_bitStream->isEof());
        return false;
    }
    
    TT_SP(MP4Box) box = MP4BoxFactory::createBox(_bitStream);
    if (box != nullptr) {
        saveBox(box);
        return true;
    }
    
    return false;
}

void MP4Parser::saveBox(TT_SP(MP4Box) box)
{
    if (!this->boxs.empty()) {
        // 非空，栈顶为 parent box
        this->boxs.top()->addChildBox(box);
    }
    
    if (box->isContainer()) {
        this->boxs.push(box);
    } else if (!this->boxs.empty() &&
               this->boxs.top()->hasAllChildBoxs()) {
        // 非空，栈顶为 parent box 且 sub box 已经添加结束，出栈
        this->boxs.pop();
    }
    
    switch (box->type()) {
        case MP4_BOX_TYPE_ftyp:
            _ftyp = std::dynamic_pointer_cast<MP4BoxFtyp>(box);
            break;
        case MP4_BOX_TYPE_moov:
            _moov = std::dynamic_pointer_cast<MP4BoxMoov>(box);
            break;
        case MP4_BOX_TYPE_mdat:
            _mdat = std::dynamic_pointer_cast<MP4BoxMdat>(box);
            break;
        case MP4_BOX_TYPE_meta:
            _meta = std::dynamic_pointer_cast<MP4BoxMeta>(box);
            break;
        default:
            break;
    }
}

