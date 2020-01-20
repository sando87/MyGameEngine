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