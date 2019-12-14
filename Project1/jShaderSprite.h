#pragma once
#include "jShader.h"

class jShaderSprite : 
	public jShader
{
public:
	jShaderSprite();
	virtual ~jShaderSprite();

	virtual bool LoadRenderResources();
	virtual bool Render();

	ShaderParamsSprite& GetParams() { return mParams; }
protected:

	bool CreateShaderAndLayout();
	bool CreateBuffers();
	bool CreateSamplerState();
	bool CreateTexture();
	bool CreateInputBuffer();

	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mMaterialBuffer;
	ID3D11Buffer* mSpriteBuffer;
	ID3D11SamplerState* mSampleState;
	ID3D11ShaderResourceView * mTextureView;
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;

	unsigned int mVertexStride;
	unsigned int mIndexCount;

	ShaderParamsSprite mParams;
};

