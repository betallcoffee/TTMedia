//
//  TTFileIO.hpp
//  TTMedia
//
//  Created by liang on 9/8/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTFileIO_hpp
#define TTFileIO_hpp

#include "TTDef.h"
#include "TTIO.hpp"

namespace TT {
    class FileIO : public IO {
    public:
        FileIO();
        ~FileIO();
        
        static bool canOpen(std::shared_ptr<URL> url);
        
        bool open(std::shared_ptr<URL> url, uint64_t offset, int flag) override;
        void close() override;
        size_t size() override;
        bool isEof() override;
        
        size_t read(uint8_t *pBuf, size_t size) override;
        size_t readAt(uint8_t *pBuf, size_t size, uint64_t pos) override;
        size_t write(const uint8_t *pBuf, size_t size) override;
        bool seek(uint64_t pos) override;
        bool seek(uint64_t pos, SeekMode mode) override;
        
        int64_t readPos() override;
        int64_t speed() override;
    
    private:
        size_t _fileSize = 0;
        
        TT_PROPERTY_DEF(FILE *, fd, nullptr)
    };
}

#endif /* TTFileIO_hpp */
