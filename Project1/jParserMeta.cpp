#include "jParserMeta.h"



jParserMeta::jParserMeta()
{
}


jParserMeta::~jParserMeta()
{
}
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

string jParserMeta::ToString()
{
	string ret = "";
	for (auto iter : mData)
		ret += iter.first + "\t\t:" + iter.second + "\r\n";

	return ret;
}

