#include "jGlobalStruct.h"
#include "jLog.h"
#include "jUtils.h"
#include "jRenderer.h"
#include "jGameObjectMgr.h"
#include <sstream>

void MyRes_CreateLayout::SetNameOffset()
{
	int numEle = head.reserve1;
	D3D11_INPUT_ELEMENT_DESC* layouts = (D3D11_INPUT_ELEMENT_DESC *)data;
	char* nameList = data + sizeof(D3D11_INPUT_ELEMENT_DESC) * numEle;
	int stringLen = head.totalSize - (sizeof(D3D11_INPUT_ELEMENT_DESC) * numEle) - sizeof(head);

	int currentIndex = 1;
	layouts[0].SemanticName = nameList;
	for (int i = 0; i < stringLen; ++i)
	{
		if (nameList[i] == '_')
		{
			nameList[i] = '\0';
			if (currentIndex < numEle)
			{
				layouts[currentIndex].SemanticName = &nameList[i + 1];
				currentIndex++;
			}
		}
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

string RenderContext::Find(void * addr)
{
	if (jGameObjectMgr::GetInst().mGPURes.find(addr) != jGameObjectMgr::GetInst().mGPURes.end())
		return "";

	stringstream ss;
	ss << addr;

	string filter = PATH_RESOURCE;
	filter += "*.dump";
	string filename = "";
	jUtils::ForEachFiles2((void*)ss.str().c_str(), filter.c_str(), [&](void* _key, string _str) {

		char* keyString = (char*)_key;
		vector<string> rets;
		jUtils::Split(_str, "_.", rets);
		if (rets[1] == string(keyString))
		{
			filename = _str;
			return false;
		}
		return true;
	});

	return filename;
}

CBMain RenderContext::FindCBMain(int _idx)
{
	CBMain ret;
	for (int i = 0;i < 8;++i)
	{
		stringstream ss;
		ss << _idx << "_" << mapUnmap[i].addr << "_b.dump";

		int fileSize = 0;
		MyRes_CreateBuffer* pData = nullptr;
		jUtils::LoadFile(PATH_RESOURCE + ss.str(), &fileSize, (char**)&pData);
		if (pData->head.totalSize == sizeof(MyRes_CreateBuffer) + sizeof(CBMain))
		{
			memcpy(&ret, pData->data, sizeof(ret));
			free(pData);
			break;
		}
		free(pData);
	}
	return ret;
}
void RenderContext::CreateResources(int _idx)
{
	int fileSize = 0;
	MyResBase *pData = nullptr;
	string filename = "";

	filename = Find(vs_addr);
	if (filename.size() > 0)
	{
		fileSize = 0;
		pData = nullptr;
		jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);
		void *pIF = ((MyRes_CreateShader*)pData)->CreateResource();
		jGameObjectMgr::GetInst().AddGPURes(pData, pIF);
	}

	filename = Find(ps_addr);
	if (filename.size() > 0)
	{
		fileSize = 0;
		pData = nullptr;
		jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);
		void *pIF = ((MyRes_CreateShader*)pData)->CreateResource();
		jGameObjectMgr::GetInst().AddGPURes(pData, pIF);
	}

	filename = Find(layout_addr);
	if (filename.size() > 0)
	{
		fileSize = 0;
		pData = nullptr;
		jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);
		MyRes_CreateShader* vsData = (MyRes_CreateShader*)jGameObjectMgr::GetInst().mGPURes[vs_addr].first;
		void *pIF = ((MyRes_CreateLayout*)pData)->CreateResource(vsData->data, vsData->head.totalSize - sizeof(MyRes_CreateShader));
		jGameObjectMgr::GetInst().AddGPURes(pData, pIF);
	}

	filename = Find(ib_addr);
	if (filename.size() > 0)
	{
		fileSize = 0;
		pData = nullptr;
		jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);
		void *pIF = ((MyRes_CreateBuffer*) pData)->CreateResource(nullptr, nullptr);
		jGameObjectMgr::GetInst().AddGPURes(pData, pIF);
	}

	for (int i = 0; i < 32; ++i)
	{
		filename = Find(vb[i].addr);
		if (filename.size() > 0)
		{
			fileSize = 0;
			pData = nullptr;
			jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);
			MyResBase* layoutData = jGameObjectMgr::GetInst().mGPURes[layout_addr].first;
			CBMain cbMain = FindCBMain(_idx);
			void *pIF = ((MyRes_CreateBuffer *)pData)->CreateResource(layoutData, &cbMain);
			jGameObjectMgr::GetInst().AddGPURes(pData, pIF);
		}

		filename = Find(ss[i].addr);
		if (filename.size() > 0)
		{
			fileSize = 0;
			pData = nullptr;
			jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);
			void *pIF = ((MyRes_CreateSS *)pData)->CreateResource();
			jGameObjectMgr::GetInst().AddGPURes(pData, pIF);
		}

		filename = Find(tex[i].addr);
		if (filename.size() > 0)
		{
			fileSize = 0;
			pData = nullptr;
			jUtils::LoadFile(PATH_RESOURCE + filename, &fileSize, (char**)&pData);

			int width = 0;
			int height = 0;
			int bufSize = 0;
			unsigned char* imgbuf = nullptr;
			if (!jUtils::LoadTarga(PATH_RESOURCE + filename + ".tga", height, width, bufSize, imgbuf))
				continue;

			void *pIF = ((MyRes_CreateTexture *)pData)->CreateResource(width, height, (char*)imgbuf);
			jGameObjectMgr::GetInst().AddGPURes(pData, pIF);

			delete[] imgbuf;
		}
	}
}
void* MyRes_CreateBuffer::CreateResource(MyResBase* _layoutInfo, CBMain* _cb)
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11Buffer *pBuf = nullptr;

	if (desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER)
	{
		MyRes_CreateLayout* pLayout = (MyRes_CreateLayout*)_layoutInfo;
		unsigned int stride = pLayout->GetStride(0);
		unsigned int textureOff = pLayout->GetTextureOffset(0);
		int vertexCount = desc.ByteWidth / stride;

		vector<VertexType_Texture> verticies;
		char* tmp = data;
		unsigned char* tex = nullptr;
		Matrix4 matTex = _cb->matTex0;
		for (int i = 0; i < vertexCount; ++i)
		{
			VertexType_Texture vert;
			memcpy(&vert.p, tmp, sizeof(Vector3f));
			tex = (unsigned char*)(tmp + textureOff);

			Vector4f tmp1;
			tmp1.x = (unsigned char)tex[1];
			tmp1.y = (unsigned char)tex[0];
			tmp1.z = (unsigned char)tex[3];
			tmp1.w = (unsigned char)tex[2];

			Vector4f tmp2;
			tmp2.x = tmp1.y * 0.003906f + tmp1.x;
			tmp2.y = tmp1.w * 0.003906f + tmp1.z;
			tmp2.x = tmp2.x * 0.5f - 64.0f;
			tmp2.y = tmp2.y * 0.5f - 64.0f;
			tmp2.z = 1.0f;

			Vector4f tmp3;
			tmp3.x = tmp2.x * matTex[0] + tmp2.y * matTex[1] + tmp2.z * matTex[3];
			tmp3.y = tmp2.x * matTex[4] + tmp2.y * matTex[5] + tmp2.z * matTex[7];
			tmp3.z = tmp2.x * matTex[8] + tmp2.y * matTex[9] + tmp2.z * matTex[11];
			tmp3.w = tmp2.x * matTex[12] + tmp2.y * matTex[13] + tmp2.z * matTex[15];

			vert.t.x = tmp3.x;
			vert.t.y = tmp3.y;

			tmp += stride;
			verticies.push_back(vert);
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType_Texture) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subRes;
		subRes.pSysMem = &verticies[0];
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		if (FAILED(pDev->CreateBuffer(&vertexBufferDesc, &subRes, &pBuf)))
			return nullptr;
	}
	else
	{
		D3D11_SUBRESOURCE_DATA subRes;
		subRes.pSysMem = data;
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		if (FAILED(pDev->CreateBuffer(&desc, &subRes, &pBuf)))
			return nullptr;
	}

	return pBuf;
}
void* MyRes_CreateLayout::CreateResource(void* _bolb, int _size)
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11InputLayout *pLayout = NULL;
	SetNameOffset();
	//int numElements = head.reserve1;
	//int stringOffset = sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements;
	//char* nameInfos = data + stringOffset;
	//vector<string> rets;
	//jUtils::Split(string(nameInfos), "_", rets);
	//for (int i = 0; i < rets.size(); ++i)
	//	pLayouts[i].SemanticName = rets[i].c_str();

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
void* MyRes_CreateTexture::CreateResource(int width, int height, char *imgTGA)
{
	auto pDev = jRenderer::GetInst().GetDevice();
	ID3D11ShaderResourceView *pTex = nullptr;
	ID3D11Texture2D* texture = nullptr;

	if (imgTGA == nullptr)
	{
		D3D11_SUBRESOURCE_DATA subRes;
		subRes.pSysMem = data;
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		HRESULT hResult = pDev->CreateTexture2D(&desc, &subRes, &texture);
		if (FAILED(hResult))
			return nullptr;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 0;

		// ÅØ½ºÃ³ÀÇ ¼ÎÀÌ´õ ¸®¼Ò½º ºä¸¦ ¸¸µì´Ï´Ù.
		hResult = pDev->CreateShaderResourceView(texture, &srvDesc, &pTex);
		if (FAILED(hResult))
			return nullptr;

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
			return nullptr;

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