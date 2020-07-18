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

class FilterGroup : public Filter {
public:
    FilterGroup();
    ~FilterGroup();
    
    virtual void addOutput(TT_SP(Filter) output, int index = 0) override;
    virtual void removeOutput(TT_SP(Filter) output, int index = 0) override;
    virtual void removeAllOutputs() override;
    
    virtual size_t width() override;
    virtual size_t height() override;
    
    /**
     * 接受输入数据
     */
    virtual void setInputFramebuffer(TT_SP(Framebuffer) frameBuffer, int index = 0) override;
    /**
     * 处理输入数据
     */
    virtual void process(int64_t timestamp, int index = 0) override;
    
    /**
     * 组装 filter
     */
    virtual void assembleFilters() = 0;
    
    void addFilter(TT_SP(Filter) filter, int index = 0);
    std::map<int, TT_SP(Filter)> filterList() { return _filterList; }
    
    void setOutput(TT_SP(Filter) output) { _output = output; }
    TT_SP(Filter) output() { return _output; }
    
protected:
    std::map<int, TT_SP(Filter)> _filterList;
    TT_SP(Filter) _output = nullptr;
};

}

#endif /* TTFilterGroup_hpp */
