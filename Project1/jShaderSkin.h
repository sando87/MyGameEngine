#pragma once
#include "jShader.h"


class jShaderSkin :
	public jShader
{
public:
	jShaderSkin();
	virtual ~jShaderSkin();

	virtual bool OnLoad();
	virtual bool OnRender();
	
	ShaderParamsSkin& GetParams() { return mParams; }
protected:

	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheMaterialBuffer(string keyName);
	ID3D11Buffer* CacheLightBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ShaderParamsSkin mParams;
};

