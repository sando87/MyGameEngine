#include "jShaderSkin.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jCaches.h"
#include "jRenderer.h"
#include "jTime.h"

#define ResName_Layout "jShaderSkin.layout"
#define ResName_Shader_Vertex "test.vs"
#define ResName_Shader_Pixel "test.ps"
#define ResName_Buffer_Matrix "jShaderSkin.cbMatrix"
#define ResName_Buffer_Material "jShaderSkin.cbMaterial"
#define ResName_Buffer_Lights "jShaderSkin.cbLights"

jShaderSkin::jShaderSkin()
{
}


jShaderSkin::~jShaderSkin()
{
}

void jShaderSkin::OnLoad()
{
	LoadDefault();
	LoadTexture(GetGameObject()->FindComponent<jImage>());
	LoadMesh(GetGameObject()->FindComponent<jMesh>());
	_warnif(mBasicMesh == nullptr);
}

bool jShaderSkin::OnRender(ObjCamera* cam)
{
	jMesh *mesh = mBasicMesh;

	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	u32 offset = 0;
	u32 vertexStride = sizeof(VertexFormatPTNIW);
	
	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	mDevContext->IASetVertexBuffers(0, 1, &vertBuf, &vertexStride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	if(indiBuf != nullptr)
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
	ShaderBufferMatrixs* dataPtr = (ShaderBufferMatrixs*)mappedResource.pData;
	//Matrix4 worldMat = GetGameObject()->GetWorldMat();
	//Matrix4 retMat = worldMat * jLine3D::ProjMatGround(Vector3(-0.1, 0.1, -1), 0);
	//dataPtr->world = retMat.transpose();
	dataPtr->world = GetGameObject()->GetWorldMat().transpose();
	dataPtr->view = cam->getPosMat_D3D().transpose();
	dataPtr->projection = cam->getProjMat().transpose();
	for (int i = 0; i<45; ++i)
		dataPtr->bones[i] = mParams.bones[i];
	mDevContext->Unmap(cbMatrix, 0);
	mDevContext->VSSetConstantBuffers(0, 1, &cbMatrix);
	
	// Material constant buffer를 잠글 수 있도록 기록한다.
	if (FAILED(mDevContext->Map(cbMatrial, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferMaterial* dataPtrMaterial = (ShaderBufferMaterial*)mappedResource.pData;
	*dataPtrMaterial = mParams.material;
	mDevContext->Unmap(cbMatrial, 0);
	mDevContext->PSSetConstantBuffers(0, 1, &cbMatrial);
	
	// light constant buffer를 잠글 수 있도록 기록한다.
	if (FAILED(mDevContext->Map(cbLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferLight* dataPtrLight = (ShaderBufferLight*)mappedResource.pData;
	*dataPtrLight = mParams.light;
	mDevContext->Unmap(cbLight, 0);
	mDevContext->PSSetConstantBuffers(1, 1, &cbLight);
	

	if (texView != nullptr)
	{
		// 픽셀 셰이더에서 셰이더 텍스처 리소스를 설정합니다.
		mDevContext->PSSetShaderResources(0, 1, &texView);
		// 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
		mDevContext->PSSetSamplers(0, 1, &sampler);
	}

	ID3D11BlendState *bs = GetAlphaOn() ? jRenderer::GetInst().GetBS_AlphaOn() : jRenderer::GetInst().GetBS_AlphaOff();
	mDevContext->OMSetBlendState(bs, nullptr, 0xffffffff);

	ID3D11DepthStencilState *dss = GetDepthOn() ? jRenderer::GetInst().GetDSS_DepthOn() : jRenderer::GetInst().GetDSS_DepthOff();
	mDevContext->OMSetDepthStencilState(dss, 1);

	// 삼각형을 그립니다.
	if(indiBuf != nullptr)
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

void jShaderSkin::LoadDefault()
{
	vertexShader	= CacheVertexShader(ResName_Shader_Vertex);
	pixelShader		= CachePixelShader(ResName_Shader_Pixel);
	layout				= CacheLayout(ResName_Layout);
	cbMatrix			= CacheMatrixBuffer(ResName_Buffer_Matrix);
	cbMatrial		= CacheMaterialBuffer(ResName_Buffer_Material);
	cbLight			= CacheLightBuffer(ResName_Buffer_Lights);
	sampler			= CacheSamplerState(ResName_SamplerState_Default);
}

void jShaderSkin::LoadMesh(jMesh * mesh)
{
	if (mesh == nullptr)
		return;

	mesh->Load();
	mBasicMesh = mesh;
	vertBuf = CacheVertexBuffer(mBasicMesh->GetFullname());
	indiBuf = CacheIndexedBuffer(mBasicMesh->GetFullname());
}

void jShaderSkin::LoadTexture(jImage * img)
{
	if (img == nullptr)
		return;

	mBasicImage = img;
	texView = CacheTextureView(mBasicImage->GetFullname());
}

ID3D11InputLayout * jShaderSkin::CacheLayout(string keyName)
{
	ID3D11InputLayout* res = (ID3D11InputLayout*)mGraphicResources->CacheResource(keyName, [this](string _name) {
		D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		polygonLayout[2].SemanticName = "NORMAL";
		polygonLayout[2].SemanticIndex = 0;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[2].InputSlot = 0;
		polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[2].InstanceDataStepRate = 0;

		polygonLayout[3].SemanticName = "INDEX";
		polygonLayout[3].SemanticIndex = 0;
		polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_UINT;
		polygonLayout[3].InputSlot = 0;
		polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[3].InstanceDataStepRate = 0;

		polygonLayout[4].SemanticName = "WEIGHT";
		polygonLayout[4].SemanticIndex = 0;
		polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[4].InputSlot = 0;
		polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[4].InstanceDataStepRate = 0;

		return CreateInputLayout(polygonLayout, 5, ResName_Shader_Vertex);
	});
	return res;
}
ID3D11Buffer * jShaderSkin::CacheMatrixBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferMatrixs);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
	return res;
}
ID3D11Buffer * jShaderSkin::CacheMaterialBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferMaterial);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
	return res;
}
ID3D11Buffer * jShaderSkin::CacheLightBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferLight);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
	return res;
}
ID3D11Buffer * jShaderSkin::CacheVertexBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string _name) {
		ID3D11Buffer *vertBuf = nullptr;
		jMesh* mesh = mBasicMesh;
		vector<VertexFormatPTNIW> vertices;
		void* vbuf = nullptr;
		u32 vbufSize = 0;
		if (mesh->GetStream())
		{
			chars stream = mesh->GetStream();
			vbuf = &stream[0];
			vbufSize = stream->size();
		}
		else
		{
			vector<VertexFormat>& meshVert = mesh->GetVerticies();
			int cnt = meshVert.size();
			for (int i = 0; i < cnt; ++i)
			{
				VertexFormatPTNIW vertex;
				vertex.p = meshVert[i].position;
				vertex.t = meshVert[i].texel;
				vertex.n = meshVert[i].normal;
				vertex.i = meshVert[i].boneIndexs;
				vertex.w = meshVert[i].weights;

				vertices.push_back(vertex);
			}
			vbuf = &vertices[0];
			vbufSize = sizeof(VertexFormatPTNIW) * vertices.size();
		}
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = vbufSize;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vbuf;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		vertBuf = CreateConstBuffer(&desc, &vertexData);
		return vertBuf;
	});
	return res;
}

