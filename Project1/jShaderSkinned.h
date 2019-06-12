#pragma once
#include "header.h"
#include "jBaseShader.h"
#include "Matrices.h"
#include "jGlobalStruct.h"
#include <vector>
using namespace std;

class jModel;
class ObjCamera;
class jTexture;

class jShaderSkinned : jBaseShader
{
public:
	jShaderSkinned();
	~jShaderSkinned();

	bool Initialize(string _vsFilename, string _psFilename);
	void Release();
	bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, jTexture * _texture, Vector4f _diffuse, Vector4f _light, vector<Matrix4>& _mats);

private:
	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mMaterialBuffer;
	ID3D11Buffer* mLightBuffer;
	ID3D11Buffer* mBoneMatBuffer;
	ID3D11SamplerState* mSampleState;

public:
	jModel *mModel;
	ID3D11ShaderResourceView * mTexture;
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;
	int mIndexCount;
	int mVertTypeSize;
	int mStartIndex;
	int mVertexOff;
	int mOffVertexOff;

	Matrix4 mWorld;
	Matrix4 mView;
	Matrix4 mProj;

	Vector4f mDiffuse;
	Vector4f mLight;

	Matrix4f mBoneMats[45];
};

