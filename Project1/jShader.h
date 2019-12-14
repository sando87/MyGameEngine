#pragma once

#include "jDirectXheader.h"
#include "junks.h"
#include "jComponent.h"
#include "jShaderHeader.h"

class jMesh;

class jShader : public jComponent
{
public:
	jShader();
	virtual ~jShader() {}

	virtual bool OnLoad() = 0;
	virtual bool OnRender() = 0;

protected:
	ID3D10Blob * CompileShader(string filename);

	ID3D11Device*			mDev;
	ID3D11DeviceContext*	mDevContext;

	ID3D11VertexShader *	mVertexShader;
	ID3D11PixelShader *		mPixelShader;

	jMesh * mMesh;

	Property_GetSetter(bool, Visiable)
	Property_GetSetter(bool, Loaded)
};

