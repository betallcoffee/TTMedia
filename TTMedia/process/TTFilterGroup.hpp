//
//  TTFilterGroup.hpp
//  TTPlayerExample
//
//  Created by liang on 27/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTFilterGroup_hpp
#define TTFilterGroup_hpp

#include <map>

#include "TTFilter.hpp"
#include "TTdef.h"

namespace TT {
class FilterGroup {
public:
    FilterGroup();
    ~FilterGroup();
    
    void addInput(TT_SP(Filter) input, int index = 0);
    std::map<int, TT_SP(Filter)> inputList() { return _inputList; }
    
    void setOutput(TT_SP(Filter) output) { _output = output; }
    TT_SP(Filter) output() { return _output; }
    
protected:
    std::map<int, TT_SP(Filter)> _inputList;
    TT_SP(Filter) _output = nullptr;
};
}

#endif /* TTFilterGroup_hpp */
