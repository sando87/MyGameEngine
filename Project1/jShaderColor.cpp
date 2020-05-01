#include "jShaderColor.h"
#include "jMesh.h"
#include "ObjCamera.h"
#include "jRenderer.h"

#define ResName_Layout "jShaderColor.layout"
#define ResName_Shader_Vertex "./shaders/color.vs"
#define ResName_Shader_Pixel "./shaders/color.ps"

jShaderColor::jShaderColor()
{
}

jShaderColor::~jShaderColor()
{
}

void jShaderColor::OnLoad()
{
	CacheCBufferBasic();
	CacheBlendState();
	CacheDepthState();
	CacheVS(ResName_Shader_Vertex);
	CachePS(ResName_Shader_Pixel);

	LoadLayout();
	LoadMesh();
	LoadCompentIndicies();
}

bool jShaderColor::OnRender(ObjCamera* cam)
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
	Draw(sizeof(VertexFormatPC), prim, vertCount, mMesh->GetIndicies().size());
	return true;
}

void jShaderColor::LoadLayout()
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	CacheInputLayout(polygonLayout, 2, ResName_Layout, ResName_Shader_Vertex);
}

void jShaderColor::LoadMesh()
{
	mMesh = GetGameObject()->FindComponent<jMesh>();
	mMesh->Load();
	string key = mMesh->GetFullname();

	vector<VertexFormatPC> vertices;
	vector<VertexFormat>& meshVert = mMesh->GetVerticies();
	int cnt = meshVert.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormatPC vertex;
		vertex.p = meshVert[i].position;
		vertex.c = meshVert[i].color;
		vertices.push_back(vertex);
	}
	CacheCBVertex(&vertices[0], sizeof(VertexFormatPC) * vertices.size(), key);
}
