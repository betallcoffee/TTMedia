//
//  TTDefForObjc.h
//  TTPlayerExample
//
//  Created by liang on 19/7/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#ifndef TTDefForObjc_h
#define TTDefForObjc_h

#define CHECK_SEL(func_name)  ([object() respondsToSelector:@selector(func_name)])

#define CALL_SEL(func_name) \
do { \
if (CHECK_SEL(func_name)) { \
[object() func_name]; \
} else { \
Filter::func_name(); \
} \
}while(0)

#define CALL_SEL_1(func_name, first) \
do { \
if (CHECK_SEL(func_name:)) { \
[object() func_name:first]; \
} else { \
Filter::func_name(first); \
} \
}while(0)

#define CALL_SEL_RETURN(func_name) \
do { \
if (CHECK_SEL(func_name)) { \
return [object() func_name]; \
} else { \
return Filter::func_name(); \
} \
}while(0)

#define CALL_SEL_RETURN_1(func_name, first) \
do { \
if (CHECK_SEL(func_name:)) { \
return [object() func_name:first]; \
} else { \
return Filter::func_name(first); \
} \
}while(0)


#endif /* TTDefForObjc_h */
