#pragma once
#include "junks.h"

#define MF_Name			"name"
#define MF_WorldPos	"worldPos"
#define MF_Mesh			"mesh"
#define MF_Shader		"shader"
#define MF_Alpha			"alpha"
#define MF_Depth			"depth"
#define MF_Anim			"anim"
#define MF_Order			"order"
#define MF_Texel			"texel"
#define MF_Img			"img"
#define MF_Child			"child"
#define MF_Item				"item"
#define MF_Type			"type"

class jParserMeta
{
public:
	jParserMeta();
	~jParserMeta();

	bool Load(string fullname);
	bool Save(string fullname);
	string ToString();
	void SetValue(string field, string value);
	template<typename Ty> void AddValue(string field, Ty value);
	template<typename Ty = string> Ty GetValue(string field);
	template<typename Ty = string> vector<Ty> GetValues(string field);
private:
	unordered_multimap<string, string> mData;
};

inline void jParserMeta::SetValue(string field, string value)
{
	if (mData.find(field) == mData.end())
		mData.insert(make_pair(field, value));
	else
		mData.find(field)->second = value;
}

template<>
inline void jParserMeta::AddValue(string field, string value)
{
	mData.insert(make_pair(field, value));
}

template<>
inline void jParserMeta::AddValue(string field, double value)
{
	string val = jUtils::ToString(value);
	mData.insert(make_pair(field, val));
}

template<>
inline void jParserMeta::AddValue(string field, int value)
{
	string val = jUtils::ToString(value);
	mData.insert(make_pair(field, val));
}

template<typename Ty>
inline void jParserMeta::AddValue(string field, Ty value)
{
	string val = value;
	mData.insert(make_pair(field, val));
}

template<>
inline string jParserMeta::GetValue(string field)
{
	auto iter = mData.find(field);
	if (iter == mData.end())
		return "";

	return iter->second;
}

template<>
inline int jParserMeta::GetValue(string field)
{
	auto iter = mData.find(field);
	if (iter == mData.end())
		return 0;

	return (int)jUtils::ToDouble(iter->second);
}

template<>
inline double jParserMeta::GetValue(string field)
{
	auto iter = mData.find(field);
	if (iter == mData.end())
		return 0;

	return jUtils::ToDouble(iter->second);
}

template<>
inline bool jParserMeta::GetValue(string field)
{
	auto iter = mData.find(field);
	if (iter == mData.end())
		return false;

	if (iter->second == "TRUE" || iter->second == "true")
		return true;

	return false;
}

template<>
inline Vector3 jParserMeta::GetValue(string field)
{
	Vector3 ret;
	auto iter = mData.find(field);
	if (iter == mData.end())
		return ret;

	strings pieces = jUtils::Split2(iter->second, " ");
	_warnif(pieces->size() != 3);

	ret.x = jUtils::ToDouble(pieces[0]);
	ret.y = jUtils::ToDouble(pieces[1]);
	ret.z = jUtils::ToDouble(pieces[2]);
	return ret;
}

template<>
inline vector<string> jParserMeta::GetValues(string field)
{
	vector<string> rets;
	auto range = mData.equal_range(field);
	for (auto iter = range.first; iter != range.second; ++iter)
		rets.push_back(iter->second);
	return rets;
}
template<>
inline vector<Vector3> jParserMeta::GetValues(string field)
{
	vector<Vector3> rets;
	auto range = mData.equal_range(field);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		Vector3 val;
		strings pieces = jUtils::Split2(iter->second, " ");
		_warnif(pieces->size() != 3);

		val.x = jUtils::ToDouble(pieces[0]);
		val.y = jUtils::ToDouble(pieces[1]);
		val.z = jUtils::ToDouble(pieces[2]);
		rets.push_back(val);
	}
	return rets;
}

