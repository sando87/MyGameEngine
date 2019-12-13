#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <stdio.h>
#include <io.h>
#include <time.h>

using namespace std;

#include "jTypeDef.h"
#include "jLog.h"
#include "jMath.h"
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


#define PATH_RESOURCE "D:\\temp\\"

enum EnumCoroutine
{
	Stop = false,
	Continue = true,
};