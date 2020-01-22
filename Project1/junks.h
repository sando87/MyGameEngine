#pragma once

#pragma warning(disable: 4200) //float map[]; 와 같은 비어있는 변수 선언시 발생
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include <tuple>
#include <memory>
#include <algorithm>
#include <utility>
#include <functional>
#include <thread>

#include <fstream>
#include <sstream>
#include <iostream>

#include <stdio.h>
#include <io.h>
#include <time.h>
#include <direct.h>

using namespace std;

#include "jLog.h"
#include "jFuncPtrList.h"
#include "Vectors.h"

#include "Matrices.h"

#include "jUtils.h"


#define Property_Getter(type, Name, def)		\
private:	type m##Name = def; \
public:	 type Get##Name() { return m##Name; }

#define Property_Setter(type, Name, def)		\
private:	type m##Name = def; \
public:	 void Set##Name(type val) { m##Name = val; }

#define Property_GetSetter(type, Name, def)		\
private:	type m##Name = def; \
public:	 type Get##Name() { return m##Name; } \
public:	 void Set##Name(type val) { m##Name = val; }

#define jPI 3.141592654
#define DegToRad(deg) ((deg)*jPI/180.0)
#define RadToDeg(rad) ((rad)*180.0/jPI)
#define IsOutofOne(x) ((0.9999<(x) && (x)<1.0001) ? false : true)

#define PATH_RESOURCES "./res/"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define CRASH_SIZE 8192
#define CRASH_STEP 8

#define ToU64(x, y) (u64)(((u64)(y) << 32) | (u64)(x))
#define ToU32x(key) (u32)(key)
#define ToU32y(key) (u32)(((u64)(key) >> 32))


#ifdef __cplusplus
#define INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU",read)
#define INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
#ifdef _WIN64
#define INITIALIZER(f) INITIALIZER2_(f,"")
#else
#define INITIALIZER(f) INITIALIZER2_(f,"_")
#endif
#else
#define INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif