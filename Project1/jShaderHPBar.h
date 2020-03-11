#pragma once
#include "jShader.h"

class ObjCamera;
class jMesh;


class jShaderHPBar :
	public jShader
{
public:
	jShaderHPBar();
	~jShaderHPBar();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

	ShaderBufferHPBars& GetParamBars() { return mParamBars; }

protected:
	void LoadLayout();
	void LoadMesh();
	void CacheCBufferBars();

	jMesh* mMesh;
	ShaderBufferHPBars		mParamBars;
	ID3D11Buffer *			mCBBars;
};

