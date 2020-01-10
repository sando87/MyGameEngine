#pragma once
#include "jShader.h"

class jImage;

class jShaderTerrain :
	public jShader
{
public:
	jShaderTerrain();
	~jShaderTerrain();

	virtual bool OnLoad();
	virtual bool OnRender();

	ShaderParamsTerrain& GetParams() { return mParams; }
protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheTexelsBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ShaderParamsTerrain mParams;

};

