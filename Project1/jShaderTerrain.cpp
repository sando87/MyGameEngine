#include "jShaderTerrain.h"
#include "ObjCamera.h"
#include "jImage.h"
#include "jMesh.h"
#include "jRenderer.h"

#define ResName_Layout "jShaderTerrain.layout"
#define ResName_Shader_Vertex "terrain.vs"
#define ResName_Shader_Pixel "terrain.ps"
#define ResName_Buffer_Matrix "jShaderTerrain.cbMatrix"
#define ResName_Buffer_Texels "jShaderTerrain.cbTexels"


jShaderTerrain::jShaderTerrain()
{
}


jShaderTerrain::~jShaderTerrain()
{
}


void jShaderTerrain::OnLoad()
{
	LoadDefault();
	vector<jImage*> imgs = GetGameObject()->FindComponents<jImage>();
	LoadTextures(imgs);
	LoadMesh(GetGameObject()->FindComponent<jMesh>());
	_warnif(mBasicMesh == nullptr);
}

bool jShaderTerrain::OnRender(ObjCamera* cam)
{
	jMesh* mesh = mBasicMesh;

	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int offset = 0;
	u32 vertexStride = sizeof(VertexFormatPT);

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	mDevContext->IASetVertexBuffers(0, 1, &vertBuf, &vertexStride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	if (indiBuf != nullptr)
		mDevContext->IASetIndexBuffer(indiBuf, DXGI_FORMAT_R32_UINT, 0);

	if (mesh->GetPrimitive() == PrimitiveMode::LineList)
		mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else if (mesh->GetPrimitive() == PrimitiveMode::TriangleList)
		mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 정점 입력 레이아웃을 설정합니다.
	mDevContext->IASetInputLayout(layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	mDevContext->VSSetShader(vertexShader, NULL, 0);
	mDevContext->PSSetShader(pixelShader, NULL, 0);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(mDevContext->Map(cbMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferWVP* dataPtr = (ShaderBufferWVP*)mappedResource.pData;
	dataPtr->world = GetGameObject()->GetTransform().getWorldMatrix().transpose();
	dataPtr->view = cam->getPosMat_D3D().transpose();
	dataPtr->projection = cam->getProjMat().transpose();
	mDevContext->Unmap(cbMatrix, 0);
	mDevContext->VSSetConstantBuffers(0, 1, &cbMatrix);

	D3D11_MAPPED_SUBRESOURCE texelMapRes;
	if (FAILED(mDevContext->Map(cbTexels, 0, D3D11_MAP_WRITE_DISCARD, 0, &texelMapRes)))
	{
		return false;
	}
	ShaderParamsTerrain* vecTexels = (ShaderParamsTerrain*)texelMapRes.pData;
	memcpy(vecTexels, &mParams.vectors[0], sizeof(Vector4f) * 12);
	mDevContext->Unmap(cbTexels, 0);
	mDevContext->VSSetConstantBuffers(1, 1, &cbTexels);

	// 픽셀 셰이더에서 셰이더 텍스처 리소스를 설정합니다.
	// 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
	for (int i = 0; i < texViews.size(); ++i)
		mDevContext->PSSetShaderResources(i, 1, &texViews[i]);

	mDevContext->PSSetSamplers(0, 1, &sampler);

	ID3D11BlendState *bs = GetAlphaOn() ? jRenderer::GetInst().GetBS_AlphaOn() : jRenderer::GetInst().GetBS_AlphaOff();
	mDevContext->OMSetBlendState(bs, nullptr, 0xffffffff);

	ID3D11DepthStencilState *dss = GetDepthOn() ? jRenderer::GetInst().GetDSS_DepthOn() : jRenderer::GetInst().GetDSS_DepthOff();
	mDevContext->OMSetDepthStencilState(dss, 1);

	// 삼각형을 그립니다.
	if (indiBuf != nullptr)
	{
		u32 indexCount = mesh->GetIndicies().size();
		mDevContext->DrawIndexed(indexCount, 0, 0);
	}
	else
	{
		u32 vertCount = mesh->GetStream() ? mesh->GetStream()->size() / vertexStride : mesh->GetVerticies().size();
		mDevContext->Draw(vertCount, 0);
	}
	return true;
}

void jShaderTerrain::LoadDefault()
{
	vertexShader	= CacheVertexShader(ResName_Shader_Vertex);
	pixelShader		= CachePixelShader(ResName_Shader_Pixel);
	layout				= CacheLayout(ResName_Layout);
	cbMatrix			= CacheMatrixBuffer(ResName_Buffer_Matrix);
	cbTexels			= CacheTexelsBuffer(ResName_Buffer_Texels);
	sampler			= CacheSamplerState(ResName_SamplerState_Default);
}
void jShaderTerrain::LoadMesh(jMesh * mesh)
{
	if (mesh == nullptr)
		return;

	mesh->Load();
	mBasicMesh = mesh;
	vertBuf = CacheVertexBuffer(mBasicMesh->GetFullname());
	indiBuf = CacheIndexedBuffer(mBasicMesh->GetFullname());
}
void jShaderTerrain::LoadTextures(vector<jImage*>& imgs)
{
	if (imgs.empty())
		return;

	for (jImage *img : imgs)
	{
		ID3D11ShaderResourceView *texView = CacheTextureView(img->GetFullname());
		if (texView != nullptr)
			texViews.push_back(texView);
	}
}
ID3D11InputLayout * jShaderTerrain::CacheLayout(string keyName)
{
	ID3D11InputLayout* res = (ID3D11InputLayout*)mGraphicResources->CacheResource(keyName, [this](string _name) {
		D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		return CreateInputLayout(polygonLayout, 2, ResName_Shader_Vertex);
	});
	return res;
}
ID3D11Buffer * jShaderTerrain::CacheMatrixBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferWVP);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
	return res;
}
ID3D11Buffer * jShaderTerrain::CacheTexelsBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderParamsTerrain);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
	return res;
}
ID3D11Buffer * jShaderTerrain::CacheVertexBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string _name) {
		jMesh* mesh = mBasicMesh;
		ID3D11Buffer *vertBuf = nullptr;
		if (mesh->GetStream())
		{
			chars stream = mesh->GetStream();
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = stream->size();
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = &stream[0];
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			vertBuf = CreateConstBuffer(&desc, &vertexData);
		}
		else
		{
			vector<VertexFormatPT> vertices;
			vector<VertexFormat>& meshVert = mesh->GetVerticies();
			int cnt = meshVert.size();
			for (int i = 0; i < cnt; ++i)
			{
				VertexFormatPT vertex;
				vertex.p = meshVert[i].position;
				vertex.t = meshVert[i].texel;

				vertices.push_back(vertex);
			}
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(VertexFormatPT) * vertices.size();
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = &vertices[0];
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			vertBuf = CreateConstBuffer(&desc, &vertexData);
		}

		return vertBuf;
	});
	return res;
}
