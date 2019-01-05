#pragma once

#include "header.h"


class jTexture
{
public:
	jTexture();
	~jTexture();

	bool Initialize(string _filename);
	void Release();

	ID3D11ShaderResourceView * mTextureView = nullptr;
};

