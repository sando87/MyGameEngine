#pragma once

#include "jDirectXheader.h"
#include "junks.h"
#include "jComponent.h"
#include "jShaderHeader.h"
#include "jGameObjectMgr.h"

class jMesh;

class jShader : public jComponent
{
public:
	jShader();
	virtual ~jShader() {}

	virtual bool OnLoad() = 0;
	virtual bool OnRender() = 0;

	ID3D11ShaderResourceView *LoadDumpTexture(string fullname);

protected:
	ID3D10Blob * CompileShader(string filename);

	ID3D11Device*			mDev;
	ID3D11DeviceContext*	mDevContext;

	ID3D11VertexShader *	mVertexShader;
	ID3D11PixelShader *		mPixelShader;

	jMesh * mMesh;
	bool mLoaded;
	friend void jGameObjectMgr::RunObjects();


	Property_GetSetter(bool, Visiable)
	Property_GetSetter(bool, AlphaOn)
	Property_GetSetter(bool, DepthOn)
	Property_GetSetter(bool, PrimitiveTriList)
};

