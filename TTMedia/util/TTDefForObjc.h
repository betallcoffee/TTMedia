//
//  TTDefForObjc.h
//  TTPlayerExample
//
//  Created by liang on 19/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTDefForObjc_h
#define TTDefForObjc_h

#define OBJC_DELEGATE(protocol_name) public: \
void setObject(id<protocol_name> object) { \
_object = object; \
} \
id<protocol_name> object() { return _object; }\
private: \
id<protocol_name> _object;

#define CHECK_SEL(sel_name)  ([object() respondsToSelector:@selector(sel_name)])

#define CALL_SEL(class_name, sel_name) \
do { \
if (CHECK_SEL(sel_name)) { \
[object() sel_name]; \
} else { \
class_name::sel_name(); \
} \
}while(0)

#define CALL_SEL_1(class_name, sel_name, first) \
do { \
if (CHECK_SEL(sel_name:)) { \
[object() sel_name:first]; \
} else { \
class_name::sel_name(first); \
} \
}while(0)

#define CALL_SEL_RETURN(class_name, sel_name) \
do { \
if (CHECK_SEL(sel_name)) { \
return [object() sel_name]; \
} else { \
return class_name::sel_name(); \
} \
}while(0)

#define CALL_SEL_RETURN_1(class_name, sel_name, first) \
do { \
if (CHECK_SEL(sel_name:)) { \
return [object() sel_name:first]; \
} else { \
return class_name::sel_name(first); \
} \
}while(0)


#endif /* TTDefForObjc_h */
