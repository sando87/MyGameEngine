#pragma once
#include "jBaseShader.h"
#include "header.h"
#include "Matrices.h"

class jModel;
class jTexture;

class jShaderTerrain :
	public jBaseShader
{
public:
	jShaderTerrain();
	~jShaderTerrain();

	bool Initialize(string _vsFilename, string _psFilename);
	void Release();
	bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, jTexture * _texture[6], Vector4f _texels[12]);

private:
	ID3D11InputLayout * mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mTexelVectorBuffer;
	ID3D11SamplerState* mSampleState;

public:
	jModel * mModel;
	ID3D11ShaderResourceView * mTexture[6];
	Matrix4 mWorld;
	Vector4f mTexelVectors[12];
};

