#pragma once
#include "jGameObject.h"

class jParserD3;

class ObjParser : public jGameObject
{
public:
	ObjParser();
	~ObjParser();

	int mFileIndex;
	int mAnimIndex;
	float mOff;
	int mPrintLogIdx = 0;
	int mPrintMatIdx = 1;

	virtual void OnLoad();

	jParserD3* mParser;

	void OnKeyDown(char ch);
	void OnKeyPressed(char ch);
	void ClipAnimation(int sIdx, int eIdx, string name, bool isLoop, string distPerLoop, float rotDeg);
	void ExportItemResource(string category);
};

