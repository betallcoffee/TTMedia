//
//  TTHTTPIO.hpp
//  TTPlayerExample
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTHTTPIO_hpp
#define TTHTTPIO_hpp

#include <pthread.h>

#include "TTMessageLoop.hpp"
#include "TTHTTPClient.hpp"
#include "TTBuffer.hpp"
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
        
        size_t read(uint8_t *pBuf, size_t size) override;
        size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos) override;
        size_t write(const uint8_t *pBuf, size_t size) override;
        bool seek(uint64_t pos) override;
        
        int64_t length() override;
        int64_t readPos() override;
        int64_t downPos() override;
        int64_t speed() override;
        
    private:
        void onDataRecived(Buffer &data);
        
        void handleMessage(std::shared_ptr<Message> message);
        MessageLoop _messageLoop;
        
        pthread_mutex_t _mutex;
        Cond _cond;
        HTTPClient _client;
        std::shared_ptr<URL> _url;
        HTTPClient::HeaderMap _headers;
        Buffer _buffer;
        
    };
}

#endif /* TTHTTPIO_hpp */
