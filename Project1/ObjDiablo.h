#pragma once

#include "jGameObject.h"
#include "jGlobalStruct.h"
#include "jParserS2.h"
#include <vector>
using namespace std;

class jModel;
class jShaderDiablo;
class jShaderColor;
class jShaderSkinned;
class jShaderTexture;
class jShaderTerrain;
class jTexture;
struct MyResBase;
struct CBMatrix;

class ObjDiablo : public jGameObject
{
public:
	ObjDiablo();
	~ObjDiablo();

private:
	float mm_x;
	void OnStart();
	void OnUpdate();
	void OnDraw();

	bool LoadCBMatrix();
	CBMatrix* GetCBMatrix(int _idx);

	jModel* mModel;
	//jShaderSkinned * mShader;
	jShaderTexture * mShader;
	//jShaderTerrain * mShader;
	jTexture * mTexture;

public:
	int mFileIndex;
	jParserD3 mRenderIfno;
	int mCBMainCount;
	char* mCBMains;
};

