#pragma once
#include "jShader.h"

class jImage;

class jShaderTerrain :
	public jShader
{
public:
	jShaderTerrain();
	~jShaderTerrain();

	virtual bool LoadRenderResources();
	virtual bool Render();

	bool SetTextures(vector<jImage *>& images);
	ShaderParamsTerrain& GetParams() { return mParams; }

protected:
	bool CreateShaderAndLayout();
	bool CreateBuffers();
	bool CreateSamplerState();
	bool CreateInputBuffer();

	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mTexelVectorBuffer;
	ID3D11SamplerState* mSampleState;
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;
	 vector<ID3D11ShaderResourceView *> mTextureViews;

	unsigned int mVertexStride;
	unsigned int mIndexCount;

	ShaderParamsTerrain mParams;

};

