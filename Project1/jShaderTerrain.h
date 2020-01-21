#pragma once
#include "jShader.h"

class jShaderTerrain :
	public jShader
{
public:
	jShaderTerrain();
	~jShaderTerrain();

	virtual void OnLoad();
	virtual bool OnRender();

	ShaderParamsTerrain& GetParams() { return mParams; }
protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheTexelsBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ShaderParamsTerrain mParams;

	ID3D11VertexShader *	vertexShader	;
	ID3D11PixelShader *		pixelShader		;
	ID3D11InputLayout *		layout				;
	ID3D11Buffer *				cbMatrix			;
	ID3D11Buffer *				cbTexels			;
	ID3D11SamplerState *	sampler			;
	ID3D11Buffer *				vertBuf			;
	ID3D11Buffer *				indiBuf			;
	vector<ID3D11ShaderResourceView *> texViews;
};

