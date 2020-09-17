//
//  TTFilterGroup.cpp
//  TTMedia
//
//  Created by liang on 27/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTFilterGroup.hpp"

using namespace TT;

FilterGroup::FilterGroup()
{
    
}

FilterGroup::~FilterGroup()
{

}

void FilterGroup::addOutput(TT_SP(Filter) output, int index)
{
    if (_sinkFilter) {
        _sinkFilter->addOutput(output, index);
    }
}

void FilterGroup::removeOutput(TT_SP(Filter) output, int index)
{
    if (_sinkFilter) {
        _sinkFilter->removeOutput(output, index);
    }
}

void FilterGroup::removeAllOutputs()
{
    if (_sinkFilter) {
        _sinkFilter->removeAllOutputs();
    }
}

size_t FilterGroup::width()
{
    if (_sinkFilter) {
        return _sinkFilter->width();
    }
    return 0;
}

size_t FilterGroup::height()
{
    if (_sinkFilter) {
        return _sinkFilter->height();
    }
    return 0;
}

void FilterGroup::setInputFramebuffer(TT_SP(Framebuffer) frameBuffer, int index)
{
    auto it = _filterList.begin();
    for ( ;it != _filterList.end(); it++) {
        if (it->second) {
            it->second->setInputFramebuffer(frameBuffer, it->first);
        }
    }
}

void FilterGroup::process(int64_t timestamp, int index)
{
    auto it = _filterList.begin();
    for ( ;it != _filterList.end(); it++) {
        if (it->second) {
            it->second->process(timestamp, it->first);
        }
    }
}

void FilterGroup::addFilter(TT_SP(Filter) filter, int index)
{
    if (filter && index >= 0) {
        _filterList[index] = filter;
    }
}
