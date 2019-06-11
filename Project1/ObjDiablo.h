#pragma once

#include "jGameObject.h"
#include "jGlobalStruct.h"
#include <vector>
using namespace std;

class jModel;
class jShaderDiablo;
class jShaderColor;
class jShaderSkinned;
class jShaderTexture;
class jTexture;
struct MyResBase;

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

	DrawingInfo DoDrawingInfo();
	void DoRenderingContext(int _index);

	jModel* mModel;
	jShaderTexture * mShader;
	jTexture * mTexture;

public:
	DrawingInfo mDrawInfos;
	RenderContext mRenderingContext;
};

