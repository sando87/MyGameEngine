#pragma once
#include "jBaseShader.h"
#include "header.h"
#include "Matrices.h"
#include "jGlobalStruct.h"

class jModel;
class ObjCamera;
class jTexture;

class jShaderTexture :
	public jBaseShader
{
public:
	jShaderTexture();
	~jShaderTexture();

	bool Initialize(string _vsFilename, string _psFilename);
	void Release();
	bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, jTexture * _texture);

private:
	ID3D11InputLayout * mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11SamplerState* mSampleState;

public:
	jModel * mModel;
	ID3D11ShaderResourceView * mTexture;
	Matrix4 mWorld;
	int mIndexCount = 0;
	int mStartIndex = 0;
	int mVertexOff = 0;
};

