#include "jParserMeta.h"



jParserMeta::jParserMeta()
{
}


jParserMeta::~jParserMeta()
{
}

//string jParserMeta::GetValue(string field)
//{
//	auto iter = mData.find(field);
//	return iter == mData.end() ? "" : iter->second;
//}
//
//strings jParserMeta::GetValues(string field)
//{
//	strings ret;
//	auto range = mData.equal_range(field);
//	for (auto iter = range.first; iter != range.second; ++iter)
//		ret->push_back(iter->second);
//	return ret;
//}

bool jParserMeta::Load(string fullname)
{
	strings lines = jUtils::LoadLines(fullname);
	for (string line : *lines)
	{
		strings infos = jUtils::Split2(line, ":\t");
		if(infos->size() == 2)
			mData.insert(make_pair(infos[0], infos[1]));
	}

	return mData.empty() ? false : true;
}
