#pragma once

#include "header.h"
struct MyResBase;

class jTexture
{
public:
	jTexture();
	~jTexture();

	bool Initialize(string _filename);
	void Release();
	bool Load_FromRes(MyResBase* _res);
	void SetShaderResourceView(ID3D11ShaderResourceView * _ptex) 
	{
		mTextureView = _ptex;
	}

	ID3D11ShaderResourceView * mTextureView = nullptr;
};

