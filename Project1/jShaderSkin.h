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
	
	ID3D11VertexShader *			vertexShader	;
	ID3D11PixelShader *				pixelShader		;
	ID3D11InputLayout *				layout				;
	ID3D11Buffer *						cbMatrix			;
	ID3D11Buffer *						cbMatrial		;
	ID3D11Buffer *						cbLight			;
	ID3D11SamplerState *			sampler			;
	ID3D11Buffer *						vertBuf			;
	ID3D11Buffer *						indiBuf			;
	ID3D11ShaderResourceView *	texView			;
};

