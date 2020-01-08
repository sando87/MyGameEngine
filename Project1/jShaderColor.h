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
	bool CreateBuffers();

	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mVertBuf;

	unsigned int mVertexStride;
	unsigned int mVertexCount;

};

