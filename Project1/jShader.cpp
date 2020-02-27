#include "jShader.h"
#include "jRenderer.h"
#include "jParserD3.h"
#include "jCaches.h"
#include "jGameObject.h"
#include "ObjCamera.h"
#include "jTransform.h"
#include "jImage.h"
#include "jMesh.h"

#define ResNameExt_Bolb ".bolb"
#define VS_FUNCNAME "jVS"
#define PS_FUNCNAME "jPS"

jShader::jShader()
{
	mDev = jRenderer::GetInst().GetDevice();
	mDevContext = jRenderer::GetInst().GetDeviceContext();
	mGraphicResources = &jGraphicResources::GetInst();
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
ID3D11Texture2D * jShader::CreateTexture2D(D3D11_TEXTURE2D_DESC * texDesc, D3D11_SUBRESOURCE_DATA * texSubData, int width, int height, unsigned char * buf)
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
void * jShader::LoadTextureRes(unsigned char * buf, int width, int height)
{
	ID3D11ShaderResourceView *texView = nullptr;
	//텍스처의 구조체를 설정합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	ID3D11Texture2D* texture = CreateTexture2D(&textureDesc, nullptr, width, height, (unsigned char *)buf);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	texView = CreateTextureView(texture, &srvDesc);
	texture->Release();
	return texView;
}
void jShader::ReleaseTextureRes(void * ptr)
{
	((ID3D11ShaderResourceView*)ptr)->Release();
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

void jShader::CacheCBufferBasic()
{
	string key = "jShader.cbBasic";
	mCBBasic = (ID3D11Buffer *)mGraphicResources->CacheResource(key, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferBasic);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
}
void jShader::CacheCBufferSkin()
{
	string key = "jShader.cbSkin";
	mCBSkin = (ID3D11Buffer *)mGraphicResources->CacheResource(key, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferSkin);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
}
void jShader::CacheSamplerState()
{
	string key = "jShader.samplerState";
	mSamplerState = (ID3D11SamplerState *)mGraphicResources->CacheResource(key, [this](string _name) {
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
}
void jShader::CacheBlendState()
{
	string key = "jShader.blendoff";
	mBlendStateOff = (ID3D11BlendState *)mGraphicResources->CacheResource(key, [&](string _name) {
		ID3D11BlendState * blendState = nullptr;
		D3D11_BLEND_DESC bsDesc;
		memset(&bsDesc, 0x00, sizeof(bsDesc));
		bsDesc.RenderTarget[0].BlendEnable = false;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (FAILED(mDev->CreateBlendState(&bsDesc, &blendState)))
			_warn();
		return blendState;
	});


	key = "jShader.blendon";
	mBlendStateOn = (ID3D11BlendState *)mGraphicResources->CacheResource(key, [&](string _name) {
		ID3D11BlendState * blendState = nullptr;
		D3D11_BLEND_DESC bsDesc;
		memset(&bsDesc, 0x00, sizeof(bsDesc));
		bsDesc.RenderTarget[0].BlendEnable = true;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (FAILED(mDev->CreateBlendState(&bsDesc, &blendState)))
			_warn();
		return blendState;
	});
}
void jShader::CacheDepthState()
{
	string key = "jShader.depthoff";
	mDepthStencilStateOff = (ID3D11DepthStencilState *)mGraphicResources->CacheResource(key, [&](string _name) {
		ID3D11DepthStencilState * depthState = nullptr;

		D3D11_DEPTH_STENCIL_DESC dssDesc;
		memset(&dssDesc, 0x00, sizeof(dssDesc));
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; //비교는 하지만 쓰지는 않음
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dssDesc.StencilEnable = false;
		dssDesc.StencilReadMask = 0xFF;
		dssDesc.StencilWriteMask = 0xFF;
		dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		if (FAILED(mDev->CreateDepthStencilState(&dssDesc, &depthState)))
			_warn();

		return depthState;
	});

	key = "jShader.depthon";
	mDepthStencilStateOn = (ID3D11DepthStencilState *)mGraphicResources->CacheResource(key, [&](string _name) {
		ID3D11DepthStencilState * depthState = nullptr;

		D3D11_DEPTH_STENCIL_DESC dssDesc;
		memset(&dssDesc, 0x00, sizeof(dssDesc));
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //비교는 하고 쓰기도 함(기본)
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dssDesc.StencilEnable = false;
		dssDesc.StencilReadMask = 0xFF;
		dssDesc.StencilWriteMask = 0xFF;
		dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		if (FAILED(mDev->CreateDepthStencilState(&dssDesc, &depthState)))
			_warn();

		return depthState;
	});
}
void jShader::CacheVS(string keyFilename)
{
	string keyFullname = "./shaders/" + keyFilename;
	mVertexShader = (ID3D11VertexShader *)mGraphicResources->CacheResource(keyFullname, [this](string _fullname) {
		ID3D11VertexShader *vsShader = nullptr;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* vertexShaderBuffer = nullptr;
		wstring filename;
		filename.assign(_fullname.begin(), _fullname.end());

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

		mGraphicResources->SetResource(_fullname + ResNameExt_Bolb, vertexShaderBuffer);
		return vsShader;
	});
}
void jShader::CachePS(string keyFilename)
{
	string keyFullname = "./shaders/" + keyFilename;
	mPixelShader = (ID3D11PixelShader *)mGraphicResources->CacheResource(keyFullname, [this](string _fullname) {
		ID3D11PixelShader *psShader = nullptr;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* pixelShaderBuffer = nullptr;
		wstring filename;
		filename.assign(_fullname.begin(), _fullname.end());
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
}
void jShader::CacheInputLayout(D3D11_INPUT_ELEMENT_DESC * layouts, int count, string keyLayout, string keyVS)
{
	mInputLayout = (ID3D11InputLayout*)mGraphicResources->CacheResource(keyLayout, [&](string _name) {
		string keyBolb = keyVS + ResNameExt_Bolb;
		ID3D10Blob * bolb = (ID3D10Blob *)mGraphicResources->GetResource(keyBolb);
		_warnif(bolb == nullptr);
		ID3D11InputLayout * layout = nullptr;
		if (FAILED(mDev->CreateInputLayout(layouts, count, bolb->GetBufferPointer(), bolb->GetBufferSize(), &layout)))
		{
			_warn();
			layout = nullptr;
		}
		return layout;
	});
}
void jShader::CacheCBVertex(void * buf, int size, string key)
{
	mCBVertex = (ID3D11Buffer *)mGraphicResources->CacheResource(key, [&](string _name) {
		ID3D11Buffer *vertBuf = nullptr;
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = buf;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		vertBuf = CreateConstBuffer(&desc, &vertexData);
		return vertBuf;
	});
}
void jShader::CacheCBIndex(void * buf, int size, string key)
{
	mCBIndex = (ID3D11Buffer *)mGraphicResources->CacheResource(key, [&](string name) {
		ID3D11Buffer *indiBuf = nullptr;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = buf;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		indiBuf = CreateConstBuffer(&desc, &vertexData);
		return indiBuf;
	});
}
void jShader::AddCachedTextureView(void * buf, int width, int height, bool isCompressed, string key)
{
	ID3D11ShaderResourceView *res = (ID3D11ShaderResourceView *)mGraphicResources->CacheResource(key, [&](string _name) {
		ID3D11ShaderResourceView *texView = nullptr;
		if (isCompressed)
		{
			MyRes_CreateTexture* rawData = (MyRes_CreateTexture*)buf;
			rawData->SetSubResMem();
			texView = (ID3D11ShaderResourceView *)rawData->CreateResource(0, 0, nullptr);
		}
		else
		{
			//텍스처의 구조체를 설정합니다.
			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.Height = height;
			textureDesc.Width = width;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			ID3D11Texture2D* texture = CreateTexture2D(&textureDesc, nullptr, width, height, (unsigned char *)buf);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			texView = CreateTextureView(texture, &srvDesc);
			texture->Release();
		}
		return texView;
	});
	
	if (res != nullptr)
		mTextureViews.push_back(res);
}

void jShader::LoadCompentIndicies()
{
	jMesh* mesh = GetGameObject()->FindComponent<jMesh>();
	mesh->Load();
	if (!mesh->GetIndicies().empty())
	{
		string key = mesh->GetFullname() + ".indicies";
		vector<u32>& stream = mesh->GetIndicies();
		CacheCBIndex(&stream[0], stream.size() * sizeof(u32), key);
	}
}

void jShader::LoadCompentTextures()
{
	vector<jImage*> images = GetGameObject()->FindComponents<jImage>();
	for (jImage* img : images)
	{
		img->Load();
		string keyFullname = img->GetFullname();
		string ext = jUtils::GetFileExtension(keyFullname);
		bool compressed = ext == "dump" ? true : false;
		AddCachedTextureView(img->GetBuffer(), img->GetWidth(), img->GetHeight(), compressed, keyFullname);
	}
}

void jShader::SetRenderContext(ObjCamera * cam)
{
	mDevContext->IASetInputLayout(mInputLayout);

	mDevContext->VSSetShader(mVertexShader, NULL, 0);
	mDevContext->PSSetShader(mPixelShader, NULL, 0);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(mDevContext->Map(mCBBasic, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		_warn();

	ShaderBufferBasic* dataPtr = (ShaderBufferBasic*)mappedResource.pData;
	*dataPtr = mParamBasic;
	dataPtr->worldMat = GetGameObject()->GetTransform().getWorldMatrix().transpose();
	dataPtr->viewMat = cam->getPosMat_D3D().transpose();
	dataPtr->projectionMat = cam->getProjMat().transpose();
	mDevContext->Unmap(mCBBasic, 0);
	mDevContext->VSSetConstantBuffers(0, 1, &mCBBasic);
	mDevContext->PSSetConstantBuffers(0, 1, &mCBBasic);

	if (mCBSkin != nullptr)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(mDevContext->Map(mCBSkin, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			_warn();

		ShaderBufferSkin* dataPtr = (ShaderBufferSkin*)mappedResource.pData;
		*dataPtr = mParamSkin;
		mDevContext->Unmap(mCBSkin, 0);
		mDevContext->VSSetConstantBuffers(1, 1, &mCBSkin);
	}


	if(!mTextureViews.empty())
		mDevContext->PSSetSamplers(0, 1, &mSamplerState);

	for(int i = 0; i< mTextureViews.size(); ++i)
		mDevContext->PSSetShaderResources(i, 1, &mTextureViews[i]);


	mDevContext->OMSetBlendState(GetAlphaOn() ? mBlendStateOn : mBlendStateOff, nullptr, 0xffffffff);

	mDevContext->OMSetDepthStencilState(GetDepthOn() ? mDepthStencilStateOn : mDepthStencilStateOff, 1);

}
void jShader::Draw(u32 stride, D3D_PRIMITIVE_TOPOLOGY primitive, u32 vertCount, u32 indiCount)
{
	u32 offset = 0;
	mDevContext->IASetVertexBuffers(0, 1, &mCBVertex, &stride, &offset);

	mDevContext->IASetPrimitiveTopology(primitive);

	//if (primitive == PrimitiveMode::LineList)
	//	mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//else if (primitive == PrimitiveMode::TriangleList)
	//	mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (mCBIndex != nullptr)
	{
		mDevContext->IASetIndexBuffer(mCBIndex, DXGI_FORMAT_R32_UINT, 0);
		mDevContext->DrawIndexed(indiCount, 0, 0);
	}
	else
	{
		mDevContext->Draw(vertCount, 0);
	}
}
