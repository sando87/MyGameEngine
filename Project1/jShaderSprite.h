#pragma once


#include "header.h"
#include "jBaseShader.h"
#include "Matrices.h"
#include <vector>
using namespace std;

class jModel;
class ObjCamera;
class jTexture;

class jShaderSprite : jBaseShader
{
private:
	struct MatrixBufferType //should be 16byte aligned
	{
		Matrix4f world;
		Matrix4f view;
		Matrix4f projection;
	};
	struct MaterialBufferType //should be 16byte aligned
	{
		Vector4f ambient;
		Vector4f diffuse;
		Vector4f specular;
		Vector4f shininess;
	};
	struct SpriteBufferType //should be 16byte aligned
	{
		Vector4f uvInfo;
	};

public:
	jShaderSprite();
	~jShaderSprite();

	bool Initialize(string _vsFilename, string _psFilename);
	void Release();
	bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, jTexture * _texture, Vector4f _diffuse, Vector2f _size, Vector2n _idx);

private:
	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mMaterialBuffer;
	ID3D11Buffer* mSpriteBuffer;
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
	Vector2n mIndexUV;
	Vector2f mSizeUV;

};

