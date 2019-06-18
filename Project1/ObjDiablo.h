#pragma once

#include "jGameObject.h"
#include "jGlobalStruct.h"
#include "jParserD3.h"
#include <vector>
using namespace std;

class jModel;
class jShaderDiablo;
class jShaderColor;
class jShaderSkinned;
class jShaderTexture;
class jTexture;
struct MyResBase;
struct CBMatrix;

struct DrawingInfo
{
	MyResBase *vertexBuffer[32];
	MyResBase *indexBuffer;
	MyResBase *ConstBuffer;
	MyResBase *vertexShader;
	MyResBase *pixelShader;
	MyResBase *texture[32];
	MyResBase *layout;
	MyResBase *sampler[32];
	unsigned int primitive;
	unsigned int IndexCount;
	unsigned int StartIndex;
	unsigned int VertexOffset;
};

class ObjDiablo : public jGameObject
{
public:
	ObjDiablo();
	~ObjDiablo();

private:
	void OnStart();
	void OnUpdate();
	void OnDraw();

	bool LoadCBMatrix();
	CBMatrix* GetCBMatrix(int _idx);

	jModel* mModel;
	//jShaderSkinned * mShader;
	jShaderTexture * mShader;
	jTexture * mTexture;

public:
	int mFileIndex;
	DrawingInfo mDrawInfos;
	jParserD3 mRenderIfno;
	int mCBMainCount;
	char* mCBMains;
};

