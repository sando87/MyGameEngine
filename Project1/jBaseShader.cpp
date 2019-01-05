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
	// �ȼ� ���̴��� �����մϴ�.
	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = nullptr;
	}

	// ���ؽ� ���̴��� �����մϴ�.
	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = nullptr;
	}
}

void jBaseShader::ErrorMsg(ID3D10Blob * _blob)
{
	// ���� �޽����� ���â�� ǥ���մϴ�.
	_echoS(reinterpret_cast<const char*>(_blob->GetBufferPointer()));

	// ���� �޼����� ��ȯ�մϴ�.
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
		// ���̴� ������ ���н� �����޽����� ����մϴ�.
		if (errorMessage)
			ErrorMsg(errorMessage);
		else
			_echoS("not found file");

		return nullptr;
	}

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	if (FAILED(jRenderer::GetInst().GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader)))
	{
		return nullptr;
	}
	return vertexShaderBuffer;
}

ID3D10Blob * jBaseShader::CompilePixelShader(string _psFilename)
{
	ID3D10Blob* errorMessage = nullptr;
	// �ȼ� ���̴� �ڵ带 �������Ѵ�.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	wstring name;
	name.assign(_psFilename.begin(), _psFilename.end());
	if (FAILED(D3DCompileFromFile(name.c_str(), NULL, NULL, PS_FUNCNAME, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���н� �����޽����� ����մϴ�.
		if (errorMessage)
			ErrorMsg(errorMessage);
		else
			_echoS("not found file");

		return nullptr;
	}

	// ���ۿ��� �ȼ� ���̴��� �����մϴ�.
	if (FAILED(jRenderer::GetInst().GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader)))
	{
		return nullptr;
	}
	return pixelShaderBuffer;
}

