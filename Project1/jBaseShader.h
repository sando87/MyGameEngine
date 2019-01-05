#ifndef __BASE_SHADER_H__
#define __BASE_SHADER_H__

#include "header.h"
#include "jRenderer.h"

#define VS_FUNCNAME "jVS"
#define PS_FUNCNAME "jPS"

class jBaseShader
{
public:
	jBaseShader();
	virtual ~jBaseShader();

public:
	virtual bool Initialize(string _vsFilename, string _psFilename) = 0;
	virtual void Release();
	virtual bool Render() = 0;

protected:
	void ErrorMsg(ID3D10Blob* _blob);
	ID3D10Blob* CompileVertexShader(string _vsFilename);
	ID3D10Blob* CompilePixelShader(string _psFilename);

	ID3D11VertexShader * mVertexShader = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;
};

#endif