//
//  TTCodec.hpp
//  TTPlayerExample
//
//  Created by liang on 2/12/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTCodec_hpp
#define TTCodec_hpp

#include <memory>

namespace TT {
    class Frame;
    class Packet;
    
    class Codec {
    public:
        Codec() {};
        virtual ~Codec() {};
        
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet) {
            return nullptr;
        };
        virtual void encode(std::shared_ptr<Frame> frame) {};
    };
}

#endif /* TTCodec_hpp */
