//
//  TTDemuxerThread.cpp
//  TTPlayerExample
//
//  Created by liang on 28/10/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTMessageLoop.hpp"
#include "TTDemuxerThread.hpp"

using namespace TT;

DemuxerControl::DemuxerControl()
{
    _loop = std::make_shared<MessageLoop>("DemuxerThread");
}

DemuxerControl::~DemuxerControl()
{
    
}

bool DemuxerControl::start() {
    return true;
}

bool DemuxerControl::stop() {
    return true;
}


