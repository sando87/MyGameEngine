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
	bool CreateShaderAndLayout();
	bool CreateBuffers();
	bool CreateSamplerState();
	bool CreateTexture();
	bool CreateInputBuffer();

	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mMaterialBuffer;
	ID3D11Buffer* mLightBuffer;
	ID3D11SamplerState* mSampleState;
	ID3D11ShaderResourceView * mTextureView;
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;

	unsigned int mVertexStride;
	unsigned int mIndexCount;

	ShaderParamsDefault mParams;
	
};