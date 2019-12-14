#ifndef __JUTILS_H__
#define __JUTILS_H__

#include <string>
#include <vector>
#include <memory>
#include <functional>
using namespace std;

class Matrix4;

template<typename T>
class shared_ptr_array : public shared_ptr<vector<T>>
{
public:
	shared_ptr_array() : shared_ptr<vector<T>>(new vector<T>()) {}
	T& operator[](int idx) { return this->get()->operator[](idx); }
	operator bool() const { return this->get()->size() > 0; }
};

typedef shared_ptr_array<string> strings;
typedef shared_ptr_array<char> chars;


struct TargaHeader
{
	unsigned char data1[12];
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char data2;
};

#define KEEPGOING true
#define STOP false


class jUtils
{
public:
	jUtils();
	~jUtils();

	static void Split(string _str, const char* _del, vector<string>& _vec);
	static strings Split2(string _str, const char* _del);
	static double ToDouble(string _str) { return stod(_str); }
	static string ToString(int _val) { return to_string(_val); }
	static chars LoadTarga(string filename, int& height, int& width, bool _isInvert = false);
	static bool LoadFile(string path, int* _bufSize, char** _buf);
	static chars LoadFile2(string path, int& _bufSize);
	static strings LoadLines(string path);
	static void ForEachFiles(void* _object, const char* _path, bool(*_func)(void *_this, char *_filename));
	static void ForEachFiles2(void* _object, const char* _path, function<bool(void*, string)> _func);
	static string FindFile(string _path, string _filter);
	static bool MyCopyFile(string _from, string _to);
	static void SaveToFile(string path, string filename, string data, bool isAttach = false);
	static string GetFileExtension(string _filename)
	{
		return _filename.substr(_filename.find_last_of(".") + 1);
	}
	static unsigned int Hash(string _str)
	{
		const char* s = _str.c_str();
		unsigned int A = 54059; /* a prime */
		unsigned int B = 76963; /* another prime */
		unsigned int C = 86969; /* yet another prime */
		unsigned int h = 37;
		while (*s) {
			h = (h * A) ^ (s[0] * B);
			s++;
		}
		return h; // or return h % C;
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