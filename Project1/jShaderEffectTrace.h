#pragma once
#include "jShader.h"

class ObjCamera;
class jMesh;

class jShaderEffectTrace :
	public jShader
{
public:
	jShaderEffectTrace();
	~jShaderEffectTrace();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

	ShaderBufferBillboards& GetParamBillboard() { return mParamBillboard; }

protected:
	void LoadLayout();
	void LoadMesh();
	void CacheCBufferBillboards();

	jMesh* mMesh;
	ShaderBufferBillboards  mParamBillboard;
	ID3D11Buffer *			mCBBillboard;
};

