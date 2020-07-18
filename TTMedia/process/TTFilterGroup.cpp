//
//  TTFilterGroup.cpp
//  TTPlayerExample
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
    if (_output) {
        _output->addOutput(output, index);
    }
}

void FilterGroup::removeOutput(TT_SP(Filter) output, int index)
{
    if (_output) {
        _output->removeOutput(output, index);
    }
}

void FilterGroup::removeAllOutputs()
{
    if (_output) {
        _output->removeAllOutputs();
    }
}

size_t FilterGroup::width()
{
    if (_output) {
        return _output->width();
    }
    return 0;
}

size_t FilterGroup::height()
{
    if (_output) {
        return _output->height();
    }
    return 0;
}

void FilterGroup::setInputFramebuffer(TT_SP(Framebuffer) frameBuffer, int index)
{
    auto it = _filterList.begin();
    if (it != _filterList.end() && it->second != nullptr) {
        it->second->setInputFramebuffer(frameBuffer, index);
    }
}

void FilterGroup::process(int64_t timestamp, int index)
{
    auto it = _filterList.begin();
    if (it != _filterList.end() && it->second != nullptr) {
        it->second->process(timestamp, index);
    }
}

void FilterGroup::addFilter(TT_SP(Filter) filter, int index)
{
    if (filter && index >= 0) {
        _filterList[index] = filter;
    }
}
