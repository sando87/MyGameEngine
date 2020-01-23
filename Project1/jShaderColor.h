#pragma once
#include "jShader.h"


class jShaderColor :
	public jShader
{
public:
	jShaderColor();
	virtual ~jShaderColor();
	
	void LoadDefault();
	void LoadMesh(jMesh * mesh);

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

protected:
	ID3D11InputLayout * CacheLayout(string keyName);
	ID3D11Buffer* CacheMatrixBuffer(string keyName);
	ID3D11Buffer* CacheVertexBuffer(string keyName);

	ID3D11VertexShader *	vertexShader;
	ID3D11PixelShader *		pixelShader;
	ID3D11InputLayout *		layout;
	ID3D11Buffer *				cbMatrix;
	ID3D11Buffer *				vertBuf;
	ID3D11Buffer *				indiBuf;
};

