#pragma once

//template <typename Ty> struct TypeName
//{
//	static const char* Get()
//	{
//		return typeid(Ty).name();
//	}
//};
//template <> struct TypeName<int> //���ø� Ư��ȭ typeid()�� �������� ������ ���
//{
//	static const char* Get()
//	{
//		return "int";
//	}
//};

template<typename Ty>
const char * TypeToString(Ty* object)
{
	const char * name = typeid(*object).name();	//"class typename" ��ȯ
	return &name[6];		//"typename"�� split�Ͽ� ��ȯ
}