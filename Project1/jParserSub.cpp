#include "jParserSub.h"
#include "jRenderer.h"

void MyRes_CreateLayout::SetNameOffset()
{
	int numEle = head.reserve1;
	D3D11_INPUT_ELEMENT_DESC* layouts = (D3D11_INPUT_ELEMENT_DESC *)data;
	for (int i = 0; i < numEle; ++i)
	{
		unsigned int pOff = (unsigned int)layouts[i].SemanticName;
		if (pOff > head.totalSize)
			return; //here means already fixed.. so return this function

		char* pBase = (char*)this;
		layouts[i].SemanticName = (LPCSTR)(pBase + pOff);
	}
}

unsigned int MyRes_CreateLayout::GetStride(int slotIndex)
{
	unsigned int stride = 0;
	D3D11_INPUT_ELEMENT_DESC *pLayout = (D3D11_INPUT_ELEMENT_DESC *)data;
	for (int i = 0; i < head.reserve1; ++i)
	{
		if (pLayout[i].InputSlot != slotIndex)
			continue;

		if (pLayout[i].Format == DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT)
			stride += sizeof(float) * 3;
		else if (pLayout[i].Format == DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT)
			stride += sizeof(char) * 4;
		else if (pLayout[i].Format == DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM)
			stride += sizeof(char) * 4;
		else
			_warn();
	}
	return stride;
}
unsigned int MyRes_CreateLayout::GetMatWeightOffset()
{
	D3D11_INPUT_ELEMENT_DESC *pLayout = (D3D11_INPUT_ELEMENT_DESC *)data;
	int numEle = head.reserve1;

	for (int i = 0; i < numEle; ++i)
	{
		if (strncmp(pLayout[i].SemanticName, "BLENDWEIGHT", 260))
			continue;

		return pLayout[i].AlignedByteOffset;
	}
	return 0;
}
unsigned int MyRes_CreateLayout::GetMatIdxOffset()
{
	D3D11_INPUT_ELEMENT_DESC *pLayout = (D3D11_INPUT_ELEMENT_DESC *)data;
	int numEle = head.reserve1;

	for (int i = 0; i < numEle; ++i)
	{
		if (strncmp(pLayout[i].SemanticName, "BLENDINDICES", 260))
			continue;

		return pLayout[i].AlignedByteOffset;
	}
	return 0;
}
unsigned int MyRes_CreateLayout::GetTextureOffset(int _index)
{
	D3D11_INPUT_ELEMENT_DESC *pLayout = (D3D11_INPUT_ELEMENT_DESC *)data;
	int numEle = head.reserve1;

	for (int i = 0; i < numEle; ++i)
	{
		if (pLayout[i].SemanticIndex != _index)
			continue;

		if (strncmp(pLayout[i].SemanticName, "TEXCOORD", 260))
			continue;

		return pLayout[i].AlignedByteOffset;
	}
	return 0;
}


void* MyRes_CreateBuffer::CreateResource()
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11Buffer *pBuf = nullptr;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = data;
	subRes.SysMemPitch = 0;
	subRes.SysMemSlicePitch = 0;
	if (FAILED(pDev->CreateBuffer(&desc, &subRes, &pBuf)))
		return nullptr;

	return pBuf;
}
void* MyRes_CreateLayout::CreateResource(void* _bolb, int _size)
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11InputLayout *pLayout = NULL;
	if (FAILED(pDev->CreateInputLayout((D3D11_INPUT_ELEMENT_DESC *)data, head.reserve1, _bolb, _size, &pLayout)))
		return nullptr;

	return pLayout;
}
void* MyRes_CreateShader::CreateResource()
{
	auto pDev = jRenderer::GetInst().GetDevice();
	if (head.type == MYRES_TYPE_CreateVS)
	{
		ID3D11VertexShader *pVS = NULL;
		if (FAILED(pDev->CreateVertexShader(data, head.totalSize - sizeof(MyRes_CreateShader), NULL, &pVS)))
			return nullptr;

		return pVS;
	}
	else if (head.type == MYRES_TYPE_CreatePS)
	{
		ID3D11PixelShader *pPS = NULL;
		if (FAILED(pDev->CreatePixelShader(data, head.totalSize - sizeof(MyRes_CreateShader), NULL, &pPS)))
			return nullptr;

		return pPS;
	}
	return nullptr;
}
void MyRes_CreateTexture::SetSubResMem()
{
	int cnt = min(GetCount(), desc.MipLevels * desc.ArraySize);
	int off = 0;
	for (int i = 0; i < cnt; ++i)
	{
		subRes[i].pSysMem = data + off;
		off += subRes[i].SysMemSlicePitch;
	}
}
void* MyRes_CreateTexture::CreateResource(int width, int height, char *imgTGA)
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11ShaderResourceView *pTex = nullptr;
	ID3D11Texture2D* texture = nullptr;

	if (imgTGA == nullptr)
	{
		if (desc.ArraySize * desc.MipLevels > GetCount())
			return nullptr;

		D3D11_SUBRESOURCE_DATA *pSubRes =
			head.totalSize == sizeof(MyRes_CreateTexture) ? nullptr : subRes;

		//desc.SampleDesc.Count = 1;
		//desc.BindFlags = D3D10_BIND_FLAG::D3D10_BIND_SHADER_RESOURCE;
		HRESULT hResult = pDev->CreateTexture2D(&desc, pSubRes, &texture);
		if (FAILED(hResult))
			return nullptr;

		D3D11_SHADER_RESOURCE_VIEW_DESC *pSrvDesc = 
			viewDesc.ViewDimension == -1 ? nullptr : &viewDesc;

		hResult = pDev->CreateShaderResourceView(texture, pSrvDesc, &pTex);
		if (FAILED(hResult))
		{
			texture->Release();
			return nullptr;
		}

	}
	else
	{
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

		HRESULT hResult = jRenderer::GetInst().GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
		if (FAILED(hResult))
			return nullptr;

		UINT rowPitch = (width * 4) * sizeof(unsigned char);
		jRenderer::GetInst().GetDeviceContext()->UpdateSubresource(texture, 0, NULL, imgTGA, rowPitch, 0);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		hResult = jRenderer::GetInst().GetDevice()->CreateShaderResourceView(texture, &srvDesc, &pTex);
		if (FAILED(hResult))
		{
			texture->Release();
			return nullptr;
		}
			

		jRenderer::GetInst().GetDeviceContext()->GenerateMips(pTex);

	}
	texture->Release();

	return pTex;
}
void* MyRes_CreateSS::CreateResource()
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11SamplerState *pSamplerState = NULL;
	if (FAILED(pDev->CreateSamplerState(&desc, &pSamplerState)))
		return nullptr;

	return pSamplerState;
}
void* MyRes_CreateBS::CreateResource()
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11BlendState *pBlendState = NULL;
	if (FAILED(pDev->CreateBlendState(&desc, &pBlendState)))
		return nullptr;

	return pBlendState;
}

void * MyRes_CreateDS::CreateResource()
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11DepthStencilState *pState = NULL;
	if (FAILED(pDev->CreateDepthStencilState(&desc, &pState)))
		return nullptr;

	return pState;
}

void * MyRes_D3D9_IB::CreateResource()
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11Buffer *pBuf = nullptr;

	D3D11_BUFFER_DESC desc11;
	desc11.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	desc11.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	desc11.ByteWidth = desc.Size;
	desc11.CPUAccessFlags = 0;
	desc11.MiscFlags = 0;
	desc11.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = data;
	subRes.SysMemPitch = 0;
	subRes.SysMemSlicePitch = 0;
	if (FAILED(pDev->CreateBuffer(&desc11, &subRes, &pBuf)))
		return nullptr;

	return pBuf;
}
int D3D9_RenderContext::GetStride()
{
	int ret = decl_eles[decl_count - 2].Offset;
	switch (decl_eles[decl_count - 2].Type)
	{
	case MyD3DDECLTYPE::D3DDECLTYPE_FLOAT3: ret += 12;	break;
	case MyD3DDECLTYPE::D3DDECLTYPE_D3DCOLOR: ret += 4;	break;
	case MyD3DDECLTYPE::D3DDECLTYPE_UBYTE4: ret += 4;	break;
	case MyD3DDECLTYPE::D3DDECLTYPE_SHORT2: ret += 8;	break;
	case MyD3DDECLTYPE::D3DDECLTYPE_SHORT4: ret += 8;	break;
	case MyD3DDECLTYPE::D3DDECLTYPE_UBYTE4N: ret += 4;	break;
	case MyD3DDECLTYPE::D3DDECLTYPE_SHORT2N: ret += 4;	break;
	default:
		_warn();
		break;
	}
	return ret;
}

int D3D9_RenderContext::GetPositionOffset(MyD3DDECLTYPE & type)
{
	for (int i = 0; i < decl_count - 1; ++i)
	{
		if (decl_eles[i].Usage == _MyD3DDECLUSAGE::D3DDECLUSAGE_POSITION)
		{
			type = (MyD3DDECLTYPE)decl_eles[i].Type;
			return decl_eles[i].Offset;
		}
	}
	return -1;
}

int D3D9_RenderContext::GetTexelOffset(int idx, MyD3DDECLTYPE& type)
{
	for (int i = 0; i < decl_count - 1; ++i)
	{
		if (decl_eles[i].Usage == _MyD3DDECLUSAGE::D3DDECLUSAGE_TEXCOORD
			&& decl_eles[i].UsageIndex == idx)
		{
			type = (MyD3DDECLTYPE)decl_eles[i].Type;
			return decl_eles[i].Offset;
		}
	}
	return -1;
}

int D3D9_RenderContext::GetNormalOffset(MyD3DDECLTYPE & type)
{
	for (int i = 0; i < decl_count - 1; ++i)
	{
		if (decl_eles[i].Usage == _MyD3DDECLUSAGE::D3DDECLUSAGE_NORMAL)
		{
			type = (MyD3DDECLTYPE)decl_eles[i].Type;
			return decl_eles[i].Offset;
		}
	}
	return -1;
}

