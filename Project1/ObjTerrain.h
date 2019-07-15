#pragma once
#include "jGameObject.h"
#include "jParserD3.h"

class jModel;
class jShaderTerrain;
class jTexture;

class ObjTerrain :
	public jGameObject
{
public:
	ObjTerrain();
	~ObjTerrain();
private:
	void OnStart();
	void OnUpdate();
	void OnDraw();


	jModel* mModel;
	jShaderTerrain * mShader;
	jTexture * mTexture[6];

public:
	float mm_x;
	int mFileIndex;
	jParserD3 mRenderIfno;
	Vector4f mTexels[12];
};

