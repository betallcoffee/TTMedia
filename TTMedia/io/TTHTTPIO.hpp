//
//  TTHTTPIO.hpp
//  TTMedia
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTHTTPIO_hpp
#define TTHTTPIO_hpp

#include "TTMessageLoop.hpp"
#include "TTHTTPClient.hpp"
#include "TTByteBuffer.hpp"
#include "TTMutex.hpp"
#include "TTCond.hpp"

#include "TTIO.hpp"

namespace TT {
    class HTTPIO : public IO {
    public:
        HTTPIO();
        ~HTTPIO();
        
        static bool canOpen(std::shared_ptr<URL> url);
        
        bool open(std::shared_ptr<URL> url, uint64_t offset, int flag) override;
        void close() override;
        size_t size() override;
        bool isEof() override;
        
        size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos) override;
        size_t write(const uint8_t *pBuf, size_t size) override;
        bool seek(uint64_t pos) override;

        int64_t speed() override;
        
    private:
        void onDataRecived(ByteBuffer &data);
        
        MessageLoop _messageLoop;
        
        
        HTTPClient _client;
        HTTPClient::HeaderMap _headers;
    };
}

#endif /* TTHTTPIO_hpp */
