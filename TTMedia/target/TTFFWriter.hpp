//
//  TTFFWriter.hpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTFFWriter_hpp
#define TTFFWriter_hpp

#include "TTPacket.hpp"
#include "TTFFMuxer.hpp"
#include "TTFilterFrame.hpp"
#include "TTCodecParams.hpp"
#include "TTVideoCodec.hpp"

namespace TT {
    class FFWriter : public FilterFrame {
    public:
        FFWriter();
        ~FFWriter();
        
        bool open(std::shared_ptr<URL> url,
                  std::shared_ptr<CodecParams> audioCodecParams,
                  std::shared_ptr<CodecParams> videoCodecParams);
        bool close();
        void flush();
        bool cancel();
        
        void processPacket(std::shared_ptr<Packet> packet);
        
        void processFrame(std::shared_ptr<Frame> frame) override;
        void process(int64_t timestamp, int index = 0) override;
        
    private:
        std::shared_ptr<FFMuxer> _muxer;
        std::shared_ptr<URL> _url;
    };
}

#endif /* TTFFWriter_hpp */
