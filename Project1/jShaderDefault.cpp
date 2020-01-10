#include "jShaderDefault.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jCaches.h"
#include "jRenderer.h"

#define ResName_Layout "jShaderDefault.layout"
#define ResName_Shader_Vertex "default.vs"
#define ResName_Shader_Pixel "default.ps"
#define ResName_Buffer_Matrix "jShaderDefault.cbMatrix"
#define ResName_Buffer_Material "jShaderDefault.cbMaterial"
#define ResName_Buffer_Lights "jShaderDefault.cbLights"

jShaderDefault::jShaderDefault()
{
}


jShaderDefault::~jShaderDefault()
{
}

bool jShaderDefault::OnLoad()
{
	return true;
}

bool jShaderDefault::OnRender()
{
	ID3D11VertexShader *vertexShader = CacheVertexShader(ResName_Shader_Vertex);
	ID3D11PixelShader *pixelShader = CachePixelShader(ResName_Shader_Pixel);
	ID3D11InputLayout *layout = CacheLayout(ResName_Layout);
	ID3D11Buffer *cbMatrix = CacheMatrixBuffer(ResName_Buffer_Matrix);
	ID3D11Buffer *cbMatrial = CacheMaterialBuffer(ResName_Buffer_Material);
	ID3D11Buffer *cbLight = CacheLightBuffer(ResName_Buffer_Lights);
	ID3D11SamplerState * sampler = CacheSamplerState(ResName_SamplerState_Default);
	string imgName = GetGameObject()->FindComponent<jImage>()->GetFullName();
	ID3D11ShaderResourceView *texView = CacheTextureView(imgName);
	jMesh* mesh = GetGameObject()->FindComponent<jMesh>();
	ID3D11Buffer *vertBuf = CacheVertexBuffer(mesh->GetName());
	ID3D11Buffer *indiBuf = CacheIndexedBuffer(mesh->GetName());

	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int offset = 0;
	u32 vertexStride = sizeof(VertexFormatPTN);

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
	dataPtr->world = GetGameObject()->GetTransport().getMatrix().transpose();
	dataPtr->view = GetGameObject()->GetCamera().getPosMat_D3D().transpose();
	dataPtr->projection = GetGameObject()->GetCamera().getProjMat().transpose();
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
	if (indiBuf != nullptr)
	{
		u32 indexCount = mesh->GetIndicies().size();
		mDevContext->DrawIndexed(indexCount, 0, 0);
	}
	else
	{
		u32 vertCount = mesh->GetVerticies().size();
		mDevContext->Draw(vertCount, 0);
	}
	return true;
}

ID3D11InputLayout * jShaderDefault::CacheLayout(string keyName)
{
	ID3D11InputLayout* res = (ID3D11InputLayout*)mGraphicResources->CacheResource(keyName, [this](string _name) {
		D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
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

		return CreateInputLayout(polygonLayout, 3, ResName_Shader_Vertex);
	});
	return res;
}
ID3D11Buffer * jShaderDefault::CacheMatrixBuffer(string keyName)
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
ID3D11Buffer * jShaderDefault::CacheMaterialBuffer(string keyName)
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
ID3D11Buffer * jShaderDefault::CacheLightBuffer(string keyName)
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
ID3D11Buffer * jShaderDefault::CacheVertexBuffer(string keyName)
{
	jGameObject* gameObj = GetGameObject();
	jMesh* mesh = gameObj->FindComponent<jMesh>();
	if (mesh == nullptr)
		return nullptr;

	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this, mesh](string _name) {
		ID3D11Buffer *vertBuf = nullptr;
		vector<VertexFormatPTN> vertices;
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
				VertexFormatPTN vertex;
				vertex.p = meshVert[i].position;
				vertex.t = meshVert[i].texel;
				vertex.n = meshVert[i].normal;

				vertices.push_back(vertex);
			}
			vbuf = &vertices[0];
			vbufSize = sizeof(VertexFormatPTN) * vertices.size();
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
