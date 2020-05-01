#include "jShaderHPBar.h"
#include "jMesh.h"
#include "jImage.h"

#define ResName_Layout "jShaderHPBars.layout"
#define ResName_Shader_Vertex "./shaders/hpbar.vs"
#define ResName_Shader_Pixel "./shaders/hpbar.ps"

jShaderHPBar::jShaderHPBar()
{
}

jShaderHPBar::~jShaderHPBar()
{
}

void jShaderHPBar::OnLoad()
{
	CacheCBufferBasic();
	CacheCBufferBars();
	CacheSamplerState();
	CacheBlendState();
	CacheDepthState();
	CacheVS(ResName_Shader_Vertex);
	CachePS(ResName_Shader_Pixel);

	LoadLayout();
	LoadMesh();
	LoadCompentIndicies();
	LoadCompentTextures();
}

bool jShaderHPBar::OnRender(ObjCamera * cam)
{
	SetRenderContext(cam);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	mDevContext->Map(mCBBars, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ShaderBufferHPBars* dataPtr = (ShaderBufferHPBars*)mappedResource.pData;
	*dataPtr = mParamBars;
	mDevContext->Unmap(mCBBars, 0);
	mDevContext->VSSetConstantBuffers(1, 1, &mCBBars);

	D3D_PRIMITIVE_TOPOLOGY prim;
	if (mMesh->GetPrimitive() == PrimitiveMode::LineList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleStrip)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	u32 vertCount = mMesh->GetVerticies().size();
	Draw(sizeof(VertexFormatPTI), prim, vertCount, mMesh->GetIndicies().size());
	return true;
}

void jShaderHPBar::LoadLayout()
{
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

	polygonLayout[2].SemanticName = "INDEX";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	CacheInputLayout(polygonLayout, 3, ResName_Layout, ResName_Shader_Vertex);
}

void jShaderHPBar::LoadMesh()
{
	mMesh = GetGameObject()->FindComponent<jMesh>();
	jMesh* mesh = mMesh;
	mesh->Load();
	string key = mesh->GetFullname();

	vector<VertexFormatPTI> vertices;
	vector<VertexFormat>& meshVert = mesh->GetVerticies();
	int cnt = meshVert.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormatPTI vertex;
		vertex.p = meshVert[i].position;
		vertex.t = meshVert[i].texel;
		vertex.i = meshVert[i].boneIndexs;
		vertices.push_back(vertex);
	}
	CacheCBVertex(&vertices[0], sizeof(VertexFormatPTI) * vertices.size(), key);
}

void jShaderHPBar::CacheCBufferBars()
{
	string key = "jShader.cbBars";
	mCBBars = (ID3D11Buffer *)mGraphicResources->CacheResource(key, [this](string name) {
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(ShaderBufferHPBars);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		return CreateConstBuffer(&desc, nullptr);
	});
}
