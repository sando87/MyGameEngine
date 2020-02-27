#pragma once
#include "jGameObject.h"
#include "jParabolic2.h"
#include "jLinear.h"

class jShaderEffectTrace;
struct ShaderBufferBillboards;

class ObjEffectFire :
	public jGameObject
{
public:
	ObjEffectFire();
	~ObjEffectFire();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	void LoadMesh();

	jShaderEffectTrace * mShader;
	ShaderBufferBillboards* mParamsBillboards;
	int mBillboardIndex;
	int mBillboardCount;
	Matrix4 mBillboardMat;
	jParabolic2 mSizes;
	jLinear mHeights;
	
};

