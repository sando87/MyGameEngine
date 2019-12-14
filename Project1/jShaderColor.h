#pragma once
#include "jShader.h"


class jShaderColor :
	public jShader
{
public:
	jShaderColor();
	virtual ~jShaderColor();

	virtual bool OnLoad();
	virtual bool OnRender();

protected:
	bool CreateShaderAndLayout();
	bool CreateInputBuffer();

	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;

	unsigned int mVertexStride;
	unsigned int mIndexCount;

	Property_Setter(bool, LineMode)
};

