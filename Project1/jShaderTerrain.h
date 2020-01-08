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
	bool CreateShaderAndLayout();
	bool CreateBuffers();
	bool CreateSamplerState();
	bool CreateInputBuffer();
	bool CreateTextures();

	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mTexelVectorBuffer;
	ID3D11SamplerState* mSampleState;
	ID3D11Buffer* mVertBuf;
	 vector<ID3D11ShaderResourceView *> mTextureViews;

	unsigned int mVertexStride;
	unsigned int mVertexCount;

	ShaderParamsTerrain mParams;

};

