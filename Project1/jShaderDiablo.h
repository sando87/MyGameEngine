#pragma once
#include "jBaseShader.h"
#include "Matrices.h"

class jModel;
class ObjCamera;
struct MyBuffer;

class jShaderDiablo :
	public jBaseShader
{
public:
	jShaderDiablo();
	~jShaderDiablo();
public:
	virtual bool Initialize(string _vsFilename, string _psFilename);
	virtual void Release();
	virtual bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, bool _isWire);

private:
	ID3D11InputLayout * mLayout;
	MyBuffer* m_pData;
	ID3D11Buffer* mMatrixBuffer;

public:
	ID3D11Buffer * mVertBuf;
	ID3D11Buffer* mIdxBuf;
	int mIndexCount;
	int mVertTypeSize;
	jModel* mModel;

	Matrix4 mWorld;
	Matrix4 mView;
	Matrix4 mProj;
	bool mIsWire;
	bool mIsIndexTwo;
};

