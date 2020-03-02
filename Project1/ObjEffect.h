#pragma once
#include "jGameObject.h"

class jShaderEffectTrace;
struct ShaderBufferBillboards;

class ObjEffect :
	public jGameObject
{
public:
	ObjEffect();
	virtual ~ObjEffect();

	void Burst(Vector3 pos, int idx);

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	void LoadMesh();
	jShaderEffectTrace * mShader;
	ShaderBufferBillboards* mParamsBillboards;
	int mBillboardCount;
	Matrix4f mBillboardMat;


	Property_Setter(string, ImgFullname,"")
	Property_Setter(Vector2f, StepUV, Vector2f(1,1))
	Property_Setter(int, BillboardIndex, 0)
};

