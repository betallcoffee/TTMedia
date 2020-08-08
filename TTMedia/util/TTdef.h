//
//  TTdef.h
//  TTPlayerExample
//
//  Created by liang on 18/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTdef_h
#define TTdef_h

#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    // android
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif


#define STRINGIZE(x) #x

static const int kInvalidValue = -1;

#define TT_PROPERTY_DECL(type, var) private: type _##var; \
public: type var(); \
public: void set##var(type var)

#define TT_PROPERTY_DECL_READONLY(type, var) private: type _##var; \
public: type var(); \
protected: void set##var(type var)

#define TT_PROPERTY_IMPL(class, type, var) type class::var() { return _##var; } \
void class::set##var(type var) { _##var = var; }

#define TT_PROPERTY_DEF(type, var) private: type _##var; \
public: type var() { return _##var; }; \
public: void set##var(type var) { _##var = var; }

#define TT_PROPERTY_DEF_READONLY(type, var) private: type _##var; \
public: type var() { return _##var; }; \
protected: void set##var(type var) { _##var = var; }

#define TT_SP(type) std::shared_ptr<type>
#define TT_MK_SP(type) std::make_shared<type>
#define TT_UP(type) std::unique_ptr<type>


#endif /* TTdef_h */
