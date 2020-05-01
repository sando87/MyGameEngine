#include "jShaderTerrain.h"
#include "ObjCamera.h"
#include "jImage.h"
#include "jMesh.h"
#include "jRenderer.h"

#define ResName_Layout "jShaderTerrain.layout"
#define ResName_Shader_Vertex "./shaders/terrain.vs"
#define ResName_Shader_Pixel "./shaders/terrain.ps"


jShaderTerrain::jShaderTerrain()
{
}


jShaderTerrain::~jShaderTerrain()
{
}


void jShaderTerrain::OnLoad()
{
	CacheCBufferBasic();
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

bool jShaderTerrain::OnRender(ObjCamera* cam)
{
	SetRenderContext(cam);

	D3D_PRIMITIVE_TOPOLOGY prim;
	if (mMesh->GetPrimitive() == PrimitiveMode::LineList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleStrip)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	u32 vertCount = mMesh->GetVerticies().size();
	Draw(sizeof(VertexFormatPT), prim, vertCount, mMesh->GetIndicies().size());
	return true;
}

void jShaderTerrain::LoadLayout()
{
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

	CacheInputLayout(polygonLayout, 2, ResName_Layout, ResName_Shader_Vertex);
}
void jShaderTerrain::LoadMesh()
{
	mMesh = GetGameObject()->FindComponent<jMesh>();
	mMesh->Load();
	string key = mMesh->GetFullname();

	vector<VertexFormatPT> vertices;
	vector<VertexFormat>& meshVert = mMesh->GetVerticies();
	int cnt = meshVert.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormatPT vertex;
		vertex.p = meshVert[i].position;
		vertex.t = meshVert[i].texel;
		vertices.push_back(vertex);
	}
	CacheCBVertex(&vertices[0], sizeof(VertexFormatPT) * vertices.size(), key);
}

