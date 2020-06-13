#pragma once

//template <typename Ty> struct TypeName
//{
//	static const char* Get()
//	{
//		return typeid(Ty).name();
//	}
//};
//template <> struct TypeName<int> //템플릿 특수화 typeid()가 지원되지 않을때 사용
//{
//	static const char* Get()
//	{
//		return "int";
//	}
//};

template<typename Ty>
const char * TypeToString(Ty* object)
{
	const char * name = typeid(*object).name();	//"class typename" 반환
	return &name[6];		//"typename"로 split하여 반환
}


#include <string>
static const unsigned int FRONT_SIZE = sizeof("TypeConversion<") - 1u;
static const unsigned int BACK_SIZE = sizeof(">::ToString") - 1u;
template <typename T>
struct TypeConversion
{
	static std::string ToString(void)
	{
		static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
		return std::string(__FUNCTION__ + FRONT_SIZE, size - 1u);
	}
};

/*
    //Usage
	class MyCCC {};
	struct MySSS {};
	string a = TypeConversion<int>::ToString();  //"int"
	string b = TypeConversion<MyCCC>::ToString(); //"class MyCCC"
	string c = TypeConversion<MySSS>::ToString(); //"struct MySSS"
*/