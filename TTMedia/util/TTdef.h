//
//  TTdef.h
//  TTPlayerExample
//
//  Created by liang on 18/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTdef_h
#define TTdef_h

#define STRINGIZE(x) #x

static const int kInvalidValue = 0;

#define TT_PROPERTY_DECL(type, var) private: type _##var; \
public: type var(); \
public: void set##var(type var)

#define TT_PROPERTY_DECL_READONLY(type, var) private: type _##var; \
public: type var(); \
private: void set##var(type var)

#define TT_PROPERTY_IMPL(class, type, var) type class::var() { return _##var; } \
void class::set##var(type var) { _##var = var; }


#endif /* TTdef_h */
