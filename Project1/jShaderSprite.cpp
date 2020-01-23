#include "jShaderSprite.h"
#include "jRenderer.h"
#include "jMesh.h"
#include "ObjCamera.h"
#include "jImage.h"

#define ResName_Layout "jShaderSprite.layout"
#define ResName_Shader_Vertex "sprite.vs"
#define ResName_Shader_Pixel "sprite.ps"
#define ResName_Buffer_Matrix "jShaderSprite.cbMatrix"
#define ResName_Buffer_Material "jShaderSprite.cbMaterial"
#define ResName_Buffer_Sprite "jShaderSprite.cbSprite"


jShaderSprite::jShaderSprite()
{
}


jShaderSprite::~jShaderSprite()
{
}

void jShaderSprite::OnLoad()
{
	LoadDefault();
	LoadTexture(GetGameObject()->FindComponent<jImage>());
	LoadMesh(GetGameObject()->FindComponent<jMesh>());
	_warnif(mBasicMesh == nullptr);
}

bool jShaderSprite::OnRender(ObjCamera* cam)
{
	jMesh * mesh = mBasicMesh;

	// ���� ������ ������ �������� �����մϴ�.
	unsigned int offset = 0;
	u32 vertexStride = sizeof(VertexFormatPT);

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	mDevContext->IASetVertexBuffers(0, 1, &vertBuf, &vertexStride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	if (indiBuf != nullptr)
		mDevContext->IASetIndexBuffer(indiBuf, DXGI_FORMAT_R32_UINT, 0);

	if (mesh->GetPrimitive() == PrimitiveMode::LineList)
		mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else if (mesh->GetPrimitive() == PrimitiveMode::TriangleList)
		mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	mDevContext->IASetInputLayout(layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	mDevContext->VSSetShader(vertexShader, NULL, 0);
	mDevContext->PSSetShader(pixelShader, NULL, 0);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
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

	// Material constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if (FAILED(mDevContext->Map(cbMatrial, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferMaterial* dataPtrMaterial = (ShaderBufferMaterial*)mappedResource.pData;
	dataPtrMaterial->ambient = mParams.ambient;
	mDevContext->Unmap(cbMatrial, 0);
	mDevContext->PSSetConstantBuffers(0, 1, &cbMatrial);

	// light constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if (FAILED(mDevContext->Map(cbSprite, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	Vector4f* dataPtrLight = (Vector4f*)mappedResource.pData;
	dataPtrLight->x = mParams.ImageIDX.x;
	dataPtrLight->y = mParams.ImageIDX.y;
	dataPtrLight->z = mParams.step.x;
	dataPtrLight->w = mParams.step.y;
	mDevContext->Unmap(cbSprite, 0);
	mDevContext->PSSetConstantBuffers(1, 1, &cbSprite);


	if (texView != nullptr)
	{
		// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
		mDevContext->PSSetShaderResources(0, 1, &texView);
		// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
		mDevContext->PSSetSamplers(0, 1, &sampler);
	}

	ID3D11BlendState *bs = GetAlphaOn() ? jRenderer::GetInst().GetBS_AlphaOn() : jRenderer::GetInst().GetBS_AlphaOff();
	mDevContext->OMSetBlendState(bs, nullptr, 0xffffffff);

	ID3D11DepthStencilState *dss = GetDepthOn() ? jRenderer::GetInst().GetDSS_DepthOn() : jRenderer::GetInst().GetDSS_DepthOff();
	mDevContext->OMSetDepthStencilState(dss, 1);

	// �ﰢ���� �׸��ϴ�.
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

void jShaderSprite::LoadDefault()
{
	vertexShader	= CacheVertexShader(ResName_Shader_Vertex);
	pixelShader		= CachePixelShader(ResName_Shader_Pixel);
	layout				= CacheLayout(ResName_Layout);
	cbMatrix			= CacheMatrixBuffer(ResName_Buffer_Matrix);
	cbMatrial		= CacheMaterialBuffer(ResName_Buffer_Material);
	cbSprite			= CacheSpriteBuffer(ResName_Buffer_Sprite);
	sampler			= CacheSamplerState(ResName_SamplerState_Default);
}
void jShaderSprite::LoadMesh(jMesh * mesh)
{
	if (mesh == nullptr)
		return;

	mesh->Load();
	mBasicMesh = mesh;
	vertBuf = CacheVertexBuffer(mBasicMesh->GetFullname());
	indiBuf = CacheIndexedBuffer(mBasicMesh->GetFullname());
}
void jShaderSprite::LoadTexture(jImage * img)
{
	if (img == nullptr)
		return;

	mBasicImage = img;
	texView = CacheTextureView(mBasicImage->GetFullname());
}
ID3D11InputLayout * jShaderSprite::CacheLayout(string keyName)
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
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		return CreateInputLayout(polygonLayout, 2, ResName_Shader_Vertex);
	});
	return res;
}
ID3D11Buffer * jShaderSprite::CacheMatrixBuffer(string keyName)
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
ID3D11Buffer * jShaderSprite::CacheMaterialBuffer(string keyName)
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
ID3D11Buffer * jShaderSprite::CacheSpriteBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vector4f);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
	return res;
}
ID3D11Buffer * jShaderSprite::CacheVertexBuffer(string keyName)
{
	ID3D11Buffer *res = (ID3D11Buffer *)mGraphicResources->CacheResource(keyName, [this](string _name) {
		ID3D11Buffer *vertBuf = nullptr;
		jMesh * mesh = mBasicMesh;
		vector<VertexFormatPT> vertices;
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
				VertexFormatPT vertex;
				vertex.p = meshVert[i].position;
				vertex.t = meshVert[i].texel;

				vertices.push_back(vertex);
			}
			vbuf = &vertices[0];
			vbufSize = sizeof(VertexFormatPT) * vertices.size();
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
