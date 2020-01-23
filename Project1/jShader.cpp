#include "jShader.h"
#include "jRenderer.h"
#include "jMesh.h"
#include "jParserD3.h"
#include "jCaches.h"
#include "jImage.h"

jShader::jShader()
{
	mDev = jRenderer::GetInst().GetDevice();
	mDevContext = jRenderer::GetInst().GetDeviceContext();
	mGraphicResources = &jGraphicResources::GetInst();

	mVertexShader = nullptr;
	mPixelShader = nullptr;

	mBasicMesh = nullptr;
	mBasicImage = nullptr;

}

ID3D11VertexShader * jShader::CacheVertexShader(string keyName)
{
	ID3D11VertexShader *res = (ID3D11VertexShader *)mGraphicResources->CacheResource(keyName, [this](string _name) {
		ID3D11VertexShader *vsShader = nullptr;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* vertexShaderBuffer = nullptr;
		wstring filename;
		filename.assign(_name.begin(), _name.end());

		if (FAILED(D3DCompileFromFile(filename.c_str(), NULL, NULL, VS_FUNCNAME, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
		{
			_echoS("failed compile VS");
			errorMessage->Release();
			return vsShader;
		}

		if (FAILED(mDev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vsShader)))
		{
			_echoS("failed create VS");
			return vsShader;
		}

		mGraphicResources->SetResource(_name + ResNameExt_Bolb, vertexShaderBuffer);
		return vsShader;
	});
	return res;

	return res;
}
ID3D11PixelShader * jShader::CachePixelShader(string keyName)
{
	ID3D11PixelShader *res = (ID3D11PixelShader *)mGraphicResources->CacheResource(keyName, [this](string _name){
		ID3D11PixelShader *psShader = nullptr;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* pixelShaderBuffer = nullptr;
		wstring filename;
		filename.assign(_name.begin(), _name.end());
		if (FAILED(D3DCompileFromFile(filename.c_str(), NULL, NULL, PS_FUNCNAME, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
		{
			_echoS("failed compile PS");
			errorMessage->Release();
			return psShader;
		}

		if (FAILED(jRenderer::GetInst().GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &psShader)))
		{
			_echoS("failed create PS");
			return psShader;
		}

		pixelShaderBuffer->Release();
		return psShader;
	});
	return res;
}
ID3D11SamplerState * jShader::CacheSamplerState(string keyName)
{
	ID3D11SamplerState *res = (ID3D11SamplerState *)mGraphicResources->CacheResource(keyName, [this](string _name) {
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		return CreateSamplerState(&desc);
	});
	return res;
}
ID3D11ShaderResourceView * jShader::CacheTextureView(string keyName)
{
	ID3D11ShaderResourceView *res = (ID3D11ShaderResourceView *)mGraphicResources->CacheResource(keyName, [this](string _name) {
		ID3D11ShaderResourceView *texView = nullptr;
		string ext = jUtils::GetFileExtension(_name);
		if (ext == "dump")
		{
			chars buf = jUtils::LoadFile2(_name);
			MyRes_CreateTexture* rawData = (MyRes_CreateTexture*)&buf[0];
			rawData->SetSubResMem();
			texView = (ID3D11ShaderResourceView *)rawData->CreateResource(0, 0, nullptr);
		}
		else if (ext == "tga")
		{
			int width = 0;
			int height = 0;
			chars buf = jUtils::LoadTarga(_name, height, width);

			//텍스처의 구조체를 설정합니다.
			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.Height = height;
			textureDesc.Width = width;
			textureDesc.MipLevels = 0;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			ID3D11Texture2D* texture = CreateTexture2D(&textureDesc, nullptr, width, height, &buf[0]);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;
			texView = CreateTextureView(texture, &srvDesc);
			texture->Release();
			return texView;
		}
		else
		{
			_echoS("unknown ext image");
		}
		return texView;
	});
	return res;
}
ID3D11Buffer * jShader::CacheIndexedBuffer(string keyName)
{
	if (mBasicMesh == nullptr || mBasicMesh->GetIndicies().size() <= 0)
		return nullptr;

	string ibKeyName = keyName + ".indexed";
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(ibKeyName, [this](string name) {
		vector<u32>& indicies = mBasicMesh->GetIndicies();
		ID3D11Buffer *indiBuf = nullptr;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = indicies.size() * sizeof(u32);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = &indicies[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		indiBuf = CreateConstBuffer(&desc, &vertexData);
		return indiBuf;
	});
	return res;
}


ID3D11InputLayout * jShader::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC * layouts, int count, string vsFileName)
{
	ID3D11InputLayout *res = nullptr;
	string bolbName = vsFileName + ResNameExt_Bolb;
	ID3D10Blob * bolb = (ID3D10Blob *)mGraphicResources->GetResource(bolbName);
	_warnif(bolb == nullptr)

	if (FAILED(mDev->CreateInputLayout(layouts, count, bolb->GetBufferPointer(), bolb->GetBufferSize(), &res)))
	{
		_warn();
		return nullptr;
	}
	return res;
}
ID3D11Buffer * jShader::CreateConstBuffer(D3D11_BUFFER_DESC* desc, D3D11_SUBRESOURCE_DATA* subData)
{
	ID3D11Buffer *buf = nullptr;
	if (FAILED(mDev->CreateBuffer(desc, subData, &buf)))
	{
		_echoS("failed CreateConstBuffer()");
		return nullptr;
	}
	return buf;
}
ID3D11SamplerState * jShader::CreateSamplerState(D3D11_SAMPLER_DESC * desc)
{
	ID3D11SamplerState *res = nullptr;
	if (FAILED(mDev->CreateSamplerState(desc, &res)))
	{
		_echoS("failed create SamplerState");
		return res;
	}
	return res;
}
ID3D11Texture2D * jShader::CreateTexture2D(D3D11_TEXTURE2D_DESC * texDesc, D3D11_SUBRESOURCE_DATA * texSubData, int width, int height, char * buf)
{
	ID3D11Texture2D* texture = nullptr;
	if (FAILED(mDev->CreateTexture2D(texDesc, texSubData, &texture)))
	{
		_echoS("failed CreateTexture2D()");
		return texture;
	}
	UINT rowPitch = (width * 4) * sizeof(unsigned char);
	mDevContext->UpdateSubresource(texture, 0, NULL, buf, rowPitch, 0);
	return texture;
}
ID3D11ShaderResourceView * jShader::CreateTextureView(ID3D11Texture2D* texture, D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc)
{
	ID3D11ShaderResourceView *texView = nullptr;
	if (FAILED(mDev->CreateShaderResourceView(texture, viewDesc, &texView)))
	{
		_echoS("failed CreateShaderResourceView()");
		return texView;
	}
	//mDevContext->GenerateMips(texView);
	return texView;
}
ID3D11ShaderResourceView * jShader::LoadDumpTexture(string name)
{
	return nullptr;
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
