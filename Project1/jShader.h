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

	virtual bool LoadRenderResources() = 0;
	virtual bool Render();

protected:
	ID3D10Blob * CompileShader(string filename);

	ID3D11Device*			mDev;
	ID3D11DeviceContext*	mDevContext;

	ID3D11VertexShader *	mVertexShader;
	ID3D11PixelShader *		mPixelShader;

	jMesh * mMesh;

	Property_Getter(bool, Loaded)
	Property_Getter(bool, Visiable)
};

