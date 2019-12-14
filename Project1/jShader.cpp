#include "jShader.h"
#include "jRenderer.h"
#include "jMesh.h"

#define VS_FUNCNAME "jVS"
#define PS_FUNCNAME "jPS"


jShader::jShader()
{
	mVisiable = true;
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mMesh = nullptr;
	mDev = jRenderer::GetInst().GetDevice();
	mDevContext = jRenderer::GetInst().GetDeviceContext();
}

ID3D10Blob * jShader::CompileShader(string filename)
{
	string vsFileName = filename + ".vs";
	string psFileName = filename + ".ps";
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	wstring name;

	name.assign(vsFileName.begin(), vsFileName.end());
	if (FAILED(D3DCompileFromFile(name.c_str(), NULL, NULL, VS_FUNCNAME, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		_echoS("failed compile VS");
		return nullptr;
	}

	if (FAILED(jRenderer::GetInst().GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader)))
	{
		_echoS("failed create VS");
		return nullptr;
	}

	name.assign(psFileName.begin(), psFileName.end());
	if (FAILED(D3DCompileFromFile(name.c_str(), NULL, NULL, PS_FUNCNAME, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		_echoS("failed compile PS");
		return nullptr;
	}

	if (FAILED(jRenderer::GetInst().GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader)))
	{
		_echoS("failed create PS");
		return nullptr;
	}

	pixelShaderBuffer->Release();
	return vertexShaderBuffer;
}
