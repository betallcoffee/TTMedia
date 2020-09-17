//
//  TTByteBuffer.hpp
//  TTMedia
//
//  Created by liang on 17/12/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTBuffer_hpp
#define TTBuffer_hpp

#include <vector>
#include <thread>

namespace TT {
    class ByteBuffer {
    public:
        ByteBuffer(size_t capacity);
        
        bool lock();
        bool unlock();
        /**
         * 这些方法非线程安全
         */
        char *beginRead() { return begin() + readIndex_; }
        char *beginWrite() { return begin() + writeIndex_; }
        size_t readableBytes() { return writeIndex_ - readIndex_; }
        size_t writeableBytes() { return buffer_.size() - writeIndex_; }
        size_t prependableBytes() { return readIndex_; }
        bool empty() { return readIndex_ == writeIndex_; }
        void reviseWriteable(size_t n) { writeIndex_ += n; }
        /**
         * Remove the n byte data from buffer.
         */
        void retrieve(size_t n) { readIndex_ += n; }
        
        const char *find(const char *sub);
        bool beginCRLF();
        const char *findCRLF();
        const char *findCRLF(char *start);
        
        void skipSpace();
        void skipCRLF();
        
        /**
         * Read a line data, and remove the data from buffer.
         */
        bool getLine(std::string &line);
        
        int readInt();
        /**
         * 后面的方法都已加锁，线程安全
         */
        void clear();
        size_t append(const char *data, size_t n);
        size_t appendBuffer(ByteBuffer &buffer);
        bool ensureWriteable(size_t n);
        
        std::string toString();
        
    private:
        static const char kCRLF[];
        char *begin() { return &*buffer_.begin(); }
        bool expendWriteable(size_t n);
        void expend(size_t size);
        bool canExpand(size_t size);
        
        pthread_mutex_t _mutex;
        std::vector<char> buffer_;
        size_t capacity_;
        size_t readIndex_ = 0;
        size_t writeIndex_ = 0;
        
    };
}

#endif /* TTBuffer_hpp */
