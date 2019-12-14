#pragma once
#include "jGameObject.h"

class jParserD3;
class jShaderParser;

class ObjTutorial1 : public jGameObject
{
public:
	ObjTutorial1();
	~ObjTutorial1();

	int mFileIndex;
	float mOff;
private:
	void OnStart();
	void OnUpdate();
	void OnDraw();

	jParserD3* mParser;
	jShaderParser* mShader;
};

