#pragma once
#include "jShader.h"

class jShaderSprite : 
	public jShader
{
public:
	jShaderSprite();
	virtual ~jShaderSprite();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

	void LoadDefault();
	void LoadMesh(jMesh * mesh);
	void LoadTexture(jImage * img);
	ShaderParamsSprite& GetParams() { return mParams; }
protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheMaterialBuffer(string keyName);
	ID3D11Buffer* CacheSpriteBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ID3D11VertexShader *				vertexShader	;
	ID3D11PixelShader *					pixelShader		;
	ID3D11InputLayout *					layout				;
	ID3D11Buffer *							cbMatrix			;
	ID3D11Buffer *							cbMatrial		;
	ID3D11Buffer *							cbSprite			;
	ID3D11SamplerState *				sampler			;
	ID3D11ShaderResourceView *		texView			;
	ID3D11Buffer *							vertBuf			;
	ID3D11Buffer *							indiBuf			;

	ShaderParamsSprite mParams;
};

