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

class jObjStarCraft :
	public jGameObject
{
public:
	jObjStarCraft();
	~jObjStarCraft();
private:
	void OnStart();
	void OnUpdate();
	void OnDraw();

	jModel* mModel;
	jShaderTexture * mShader;
	jTexture * mTexture;

public:
	float mm_x;
	int mFileIndex;
	jParserS2 mRenderIfno;
};

