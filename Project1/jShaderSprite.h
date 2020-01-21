#pragma once
#include "jShader.h"

class jShaderSprite : 
	public jShader
{
public:
	jShaderSprite();
	virtual ~jShaderSprite();

	virtual void OnLoad();
	virtual bool OnRender();

	ShaderParamsSprite& GetParams() { return mParams; }
protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheMaterialBuffer(string keyName);
	ID3D11Buffer* CacheSpriteBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ShaderParamsSprite mParams;
};

