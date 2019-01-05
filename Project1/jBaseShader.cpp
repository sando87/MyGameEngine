#include "jBaseShader.h"
#include "jLog.h"


jBaseShader::jBaseShader()
{
}


jBaseShader::~jBaseShader()
{
	Release();
}

void jBaseShader::Release()
{
	// 픽셀 쉐이더를 해제합니다.
	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = nullptr;
	}

	// 버텍스 쉐이더를 해제합니다.
	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = nullptr;
	}
}

void jBaseShader::ErrorMsg(ID3D10Blob * _blob)
{
	// 에러 메시지를 출력창에 표시합니다.
	_echoS(reinterpret_cast<const char*>(_blob->GetBufferPointer()));

	// 에러 메세지를 반환합니다.
	_blob->Release();
	_blob = nullptr;

	_error();
}

ID3D10Blob * jBaseShader::CompileVertexShader(string _vsFilename)
{
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	wstring name;
	name.assign(_vsFilename.begin(), _vsFilename.end());
	if (FAILED(D3DCompileFromFile(name.c_str(), NULL, NULL, VS_FUNCNAME, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
			ErrorMsg(errorMessage);
		else
			_echoS("not found file");

		return nullptr;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	if (FAILED(jRenderer::GetInst().GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader)))
	{
		return nullptr;
	}
	return vertexShaderBuffer;
}

ID3D10Blob * jBaseShader::CompilePixelShader(string _psFilename)
{
	ID3D10Blob* errorMessage = nullptr;
	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	wstring name;
	name.assign(_psFilename.begin(), _psFilename.end());
	if (FAILED(D3DCompileFromFile(name.c_str(), NULL, NULL, PS_FUNCNAME, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
			ErrorMsg(errorMessage);
		else
			_echoS("not found file");

		return nullptr;
	}

	// 버퍼에서 픽셀 쉐이더를 생성합니다.
	if (FAILED(jRenderer::GetInst().GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader)))
	{
		return nullptr;
	}
	return pixelShaderBuffer;
}

