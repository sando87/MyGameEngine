#include "jShader.h"
#include "jRenderer.h"
#include "jMesh.h"
#include "jParserD3.h"
#include "jCaches.h"

#define VS_FUNCNAME "jVS"
#define PS_FUNCNAME "jPS"


jShader::jShader()
{
	memset(this, 0x00, sizeof(jShader));
	
	mVisiable = true;
	mDepthOn = true;
	mPrimitiveTriList = true;

	mDev = jRenderer::GetInst().GetDevice();
	mDevContext = jRenderer::GetInst().GetDeviceContext();
}

ID3D11ShaderResourceView * jShader::LoadDumpTexture(string fullname)
{
	ID3D11ShaderResourceView *pTexView = nullptr;
	pTexView = (ID3D11ShaderResourceView *) jCaches::CacheGraphics(fullname, [](string _fullname) {
		chars buf = jUtils::LoadFile2(_fullname);
		MyRes_CreateTexture* rawData = (MyRes_CreateTexture*)&buf[0];
		rawData->SetSubResMem();
		return (ID3D11ShaderResourceView *)rawData->CreateResource(0, 0, nullptr);
	});
	return pTexView;
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
