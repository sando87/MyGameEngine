#pragma once
#include "jShader.h"

class jShaderDefault :
	public jShader
{
public:
	jShaderDefault();
	virtual ~jShaderDefault();

	virtual bool OnLoad();
	virtual bool OnRender();

	ShaderParamsDefault& GetParams() { return mParams; }

protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheMaterialBuffer(string keyName);
	ID3D11Buffer* CacheLightBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ShaderParamsDefault mParams;
	
};