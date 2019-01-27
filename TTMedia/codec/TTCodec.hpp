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
    
    enum class CodecType{
        kNone,
        kDecode,
        kEncode
    };
    
    class Codec {
    public:
        Codec() {};
        virtual ~Codec() {};
        
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet) {
            return nullptr;
        };
        virtual bool encode(std::shared_ptr<Frame> frame) { return false; };
    };
}

#endif /* TTCodec_hpp */
