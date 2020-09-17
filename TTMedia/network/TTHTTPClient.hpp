//
//  TTHTTPClient.hpp
//  TTMedia
//
//  Created by liang on 26/11/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTHTTPClient_hpp
#define TTHTTPClient_hpp

#include <string>
#include <map>
#include <vector>
#include <pthread.h>

#include "TTCond.hpp"
#include "TTByteBuffer.hpp"
#include "TTURL.hpp"
#include "TTTCPSocket.hpp"

namespace TT {
    class HTTPClient {
    public:
        HTTPClient();
        ~HTTPClient();
        
        typedef std::map<std::string, std::string> HeaderMap;
        typedef std::function<void(ByteBuffer &data)> DataRecivedCallback;
        typedef std::function<void()> ErrorCallback;
        void Get(std::shared_ptr<URL> url, const HeaderMap &headers, DataRecivedCallback dataCallback, ErrorCallback errorCallback);
        void cancel();
        
        int statusCode() { return _statusCode; }
        int64_t contentLength() { return _contentLength; }
        
    private:
        void skipToNext();
        
        void resetResponse();
        bool process(const char *data, size_t size, DataRecivedCallback dataCallback, ErrorCallback errorCallback);
        bool parserFirstLine();
        bool parserHeaders();
        
        std::shared_ptr<TCPSocket> _socket;
        
        pthread_mutex_t _mutex;
        Cond _cond;
        bool _isCanceled;
        
        enum {
            kMaxBufferSize = 1024 * 1024,
            kMaxBytesSize = 10 * 1024,
        };
        ByteBuffer _buffer;
        char _bytes[kMaxBytesSize];
        
        enum ParseStatus {
            kParseFirstLine,
            kParseHeader,
            kParseBody,
            kParseEnd,
        };
        ParseStatus _parseStatus;
        HeaderMap _responseHeaders;
        
        std::string _httpVer;
        int _statusCode;
        std::string _phase;
        
        bool _isClose;
        bool _isChunked;
        std::string _contentType;
        int64_t _contentLength;
    };
}

#endif /* TTHTTPClient_hpp */
