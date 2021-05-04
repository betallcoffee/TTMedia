//
//  TTIO.hpp
//  TTMedia
//
//  Created by liang on 4/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTIO_hpp
#define TTIO_hpp

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "TTURL.hpp"
#include "TTCond.hpp"
#include "TTByteBuffer.hpp"

namespace TT {
    class IO {
    public:
        IO();
        virtual ~IO();
        
        static bool canOpen(std::shared_ptr<URL> url);
        static std::shared_ptr<IO> createIO(std::shared_ptr<URL> url);
        
        virtual bool open(std::shared_ptr<URL> url, uint64_t offset, int flag) = 0;
        virtual void close() = 0;
        virtual size_t size() = 0;
        /**
         * 保证读到的数据大小为 size
         * 否则返回 0
         */
        virtual size_t read(uint8_t *pBuf, size_t size);
        virtual size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos) = 0;
        virtual size_t write(const uint8_t *pBuf, size_t size) = 0;
        virtual bool seek(uint64_t pos) = 0;
        
        virtual int64_t readPos() { return _readPos; };
        virtual int64_t canReadPos() { return _canReadPos; };
        virtual int64_t readAvailable() { return canReadPos() - readPos(); };
        virtual int64_t speed() = 0;
        
    protected:
        pthread_mutex_t _mutex;
        bool _isOpened;
        std::shared_ptr<URL> _url;
        
        int _flag;
        uint64_t _readPos;
        uint64_t _canReadPos;
        
        Cond _cond;
        ByteBuffer _buffer;
    
    };
}

#endif /* TTIO_hpp */
