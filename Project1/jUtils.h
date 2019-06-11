#ifndef __JUTILS_H__
#define __JUTILS_H__

#include "Matrices.h"
#include <string>
#include <vector>
#include <functional>
using namespace std;

struct TargaHeader
{
	unsigned char data1[12];
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char data2;
};

class jUtils
{
public:
	jUtils();
	~jUtils();

	static void GetPerspectiveFovLH(Matrix4& _mat, double _fovDeg, double _aspect, double _near, double _far);
	static void Split(string _str, const char* _del, vector<string>& _vec);
	static bool LoadTarga(string filename, int& height, int& width, int& _bufSize, unsigned char*& _buf);
	static bool LoadFile(string path, int* _bufSize, char** _buf);
	static void ForEachFiles(void* _object, const char* _path, bool(*_func)(void *_this, char *_filename));
	static void ForEachFiles2(void* _object, const char* _path, function<bool(void*, string)> _func);
	static string GetFileExtension(string _filename)
	{
		return _filename.substr(_filename.find_last_of(".") + 1);
	}
	static inline string& ltrim(string& s, const char* t = " \t\n\r\f\v")
	{
		s.erase(0, s.find_first_not_of(t));
		return s;
	}
	static inline string& rtrim(string& s, const char* t = " \t\n\r\f\v")
	{
		s.erase(s.find_last_not_of(t) + 1);
		return s;
	}
	static inline string& trim(string& s, const char* t = " \t\n\r\f\v")
	{
		return ltrim(rtrim(s, t), t);
	}
};

#endif