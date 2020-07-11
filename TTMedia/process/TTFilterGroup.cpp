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

void FilterGroup::addInput(TT_SP(Filter) input, int index)
{
    if (input && index >= 0) {
        _inputList[index] = input;
    }
}
