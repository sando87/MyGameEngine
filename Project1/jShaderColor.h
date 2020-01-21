#pragma once
#include "jShader.h"


class jShaderColor :
	public jShader
{
public:
	jShaderColor();
	virtual ~jShaderColor();

	virtual void OnLoad();
	virtual bool OnRender();

protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

};

