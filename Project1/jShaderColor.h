#pragma once
#include "jBaseShader.h"
#include "Matrices.h"
#include "jGlobalStruct.h"

class jModel;
class ObjCamera;


class jShaderColor :
	public jBaseShader
{
public:
	jShaderColor();
	virtual ~jShaderColor();

	bool Initialize(string _vsFilename, string _psFilename);
	void Release();
	bool Render();

	void SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, bool _isWire);

private:
	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;

public:
	ID3D11Buffer* mVertBuf;
	ID3D11Buffer* mIdxBuf;
	int mIndexCount;
	int mVertTypeSize;

	Matrix4 mWorld;
	Matrix4 mView;
	Matrix4 mProj;
	bool mIsWire;
	bool mIsIndexTwo;
};

