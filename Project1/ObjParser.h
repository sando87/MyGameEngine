#pragma once
#include "jGameObject.h"

class jParserD3;
class jShaderParser;

class ObjParser : public jGameObject
{
public:
	friend class jEventParser;

	ObjParser();
	~ObjParser();

	int mFileIndex;
	float mOff;

	virtual void OnStart();

	jParserD3* mParser;
	jShaderParser* mShader;

};

