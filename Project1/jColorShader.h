#pragma once
#include "header.h"
#include "jBaseShader.h"
#include "Matrices.h"
#include <vector>
using namespace std;

class jModel;
class jCamera;
class jTexture;

class jColorShader : jBaseShader
{
private:
	struct MatrixBufferType //should be 16byte aligned
	{
		Matrix4f world;
		Matrix4f view;
		Matrix4f projection;
		Matrix4f bones[43];
	};
	struct MaterialBufferType //should be 16byte aligned
	{
		Vector4f ambient;
		Vector4f diffuse;
		Vector4f specular;
		Vector4f shininess;
	};
	struct LightBufferType //should be 16byte aligned
	{
		Vector4f position;
		Vector4f direction;
		Vector4f color;
		Vector4f reserve;
	};

public:
	jColorShader();
	~jColorShader();

	bool Initialize(string _vsFilename, string _psFilename);
	void Release();
	bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, jCamera * _camera, jTexture * _texture, Vector4f _diffuse, Vector4f _light, vector<Matrix4>& _mats);

private:
	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mMaterialBuffer;
	ID3D11Buffer* mLightBuffer;
	ID3D11Buffer* mBoneMatBuffer;
	ID3D11SamplerState* mSampleState;

public:
	ID3D11ShaderResourceView * mTexture;
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;
	int mIndexCount;
	int mVertTypeSize;

	Matrix4 mWorld;
	Matrix4 mView;
	Matrix4 mProj;

	Vector4f mDiffuse;
	Vector4f mLight;

	Matrix4f mBoneMats[43];
};

