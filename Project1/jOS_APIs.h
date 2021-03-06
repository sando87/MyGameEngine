#pragma once
#include <string>
#include <functional>
#include "Vectors.h"
using namespace std;

class jOS_APIs
{
public:
	jOS_APIs();
	~jOS_APIs();

	static void SetHandle(void *hwnd);
	static void ForEachFiles(void* _object, const char* _path, function<bool(void*, string)> _func);
	static void MyCopyFile(string _from, string _to);
	static Vector2 GetCursorScreenPos();
	
};

