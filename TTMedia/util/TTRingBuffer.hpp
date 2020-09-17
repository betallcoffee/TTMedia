//
//  TTRingBuffer.hpp
//  TTMedia
//
//  Created by liang on 2/9/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTRingBuffer_hpp
#define TTRingBuffer_hpp

#include <vector>

#include "TTMutex.hpp"

namespace TT {
    
    template<class T>
    class RingBuffer {
    public:
        RingBuffer(size_t maxSize) : _maxSize(maxSize) {
            _data.resize(maxSize);
        }
        ~RingBuffer() {
            
        }
        
        size_t size() {
            if (_write > _read) {
                return _write - _read;
            } else {
                return _maxSize - _read + _write;
            }
        }
        
        bool isEmpty() {
            if (_read == _write) {
                return true;
            } else {
                return false;
            }
        }
        bool isFull() {
            if (_read == _write + 1) {
                return true;
            } else {
                return false;
            }
        }
        
        void push(T elm) {
            if (!isFull()) {
                _data[_write] = elm;
                _write = (_write + 1) % _maxSize;
            }
        }
        
        T pop() {
            if (!isEmpty()) {
                T elm = _data[_read];
                _read = (_read + 1) % _maxSize;
                return elm;
            } else {
                return static_cast<T>(0);
            }
        }
        
    private:
        size_t _read;
        size_t _write;
        size_t _maxSize;
        std::vector<T> _data;
    };
}

#endif /* TTRingBuffer_hpp */
