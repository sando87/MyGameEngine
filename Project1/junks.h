#pragma once

#pragma warning(disable: 4244)
#pragma warning(disable: 4267)

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include <memory>
#include <algorithm>
#include <utility>
#include <functional>

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


#define Property_Getter(type, Name)		\
protected: type m##Name; \
public:	 type Get##Name() { return m##Name; }

#define Property_Setter(type, Name)		\
protected: type m##Name; \
public:	 void Set##Name(type val) { m##Name = val; }

#define Property_GetSetter(type, Name)		\
protected: type m##Name; \
public:	 type Get##Name() { return m##Name; } \
public:	 void Set##Name(type val) { m##Name = val; }

#define jPI 3.141592654
#define DegToRad(deg) ((deg)*jPI/180.0)
#define RadToDeg(rad) ((rad)*180.0/jPI)
#define IsOutofOne(x) ((0.9999<(x) && (x)<1.0001) ? false : true)

#define PATH_RESOURCE "D:\\temp\\"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define CoroutineKeep true
#define CoroutineStop false

#define CRASH_SIZE 8192
#define CRASH_STEP 8