#pragma once


#include <memory>
#include <string>
#include <vector>
using namespace std;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

template<typename T>
class shared_ptr_array : public shared_ptr<vector<T>>
{
public: 
	//shared_ptr<vector<T>> smart_ptr;
	//shared_ptr_array() { obj = make_shared<vector<T>>(); }
	//shared_ptr_array(vector<T>* _obj) : obj(_obj) {}
	//shared_ptr_array<T>::shared_ptr_array() {}
	shared_ptr_array() : shared_ptr<vector<T>>(new vector<T>()){}
	T& operator[](int idx) { return this->get()->operator[](idx); }
	//vector<T>* operator->() { return smart_ptr.get(); }
	//vector<T>& operator*() { return *smart_ptr; }
	operator bool() const { return this->get()->size() > 0; }
};


typedef shared_ptr_array<string> strings;
//typedef shared_ptr<vector<string>> strings;
typedef shared_ptr_array<char> chars;


