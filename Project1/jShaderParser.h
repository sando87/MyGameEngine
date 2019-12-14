#pragma once
#include "jShader.h"

class jParserD3;

class jShaderParser :
	public jShader
{
public:
	jShaderParser();
	virtual ~jShaderParser();

	virtual bool LoadRenderResources();
	virtual bool Render();

	void Load(jParserD3* parser);

private:
	ID3D11InputLayout * mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11SamplerState* mSampleState;
	ID3D11ShaderResourceView * mTexture;
	ID3D11Buffer *mVertBuffer;
	ID3D11Buffer *mIndexBuffer;

	unsigned int mIASetVertexBuffersStride;
	unsigned int mIASetVertexBuffersOff;
	int mIndexStride; // 2 or 4
	int mPrimitiveTopology;
	int mDrawIndexed1;
	int mDrawIndexed2;
	int mDrawIndexed3;

};

