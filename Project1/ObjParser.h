#pragma once
#include "jGameObject.h"

class jParserD3;
class jShaderParser;

class ObjParser : public jGameObject
{
public:
	ObjParser();
	~ObjParser();

	int mFileIndex;
	float mOff;

	virtual void OnStart();

	jParserD3* mParser;
	jShaderParser* mShader;

	int animIndex = 0;
};
