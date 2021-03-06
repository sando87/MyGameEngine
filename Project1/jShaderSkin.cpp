#include "jShaderSkin.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jCaches.h"
#include "jRenderer.h"
#include "jTime.h"

#define ResName_Layout "jShaderSkin.layout"
#define ResName_Shader_Vertex "./shaders/skin.vs"
#define ResName_Shader_Pixel "./shaders/skin.ps"

jShaderSkin::jShaderSkin()
{
}


jShaderSkin::~jShaderSkin()
{
}

void jShaderSkin::OnLoad()
{
	CacheCBufferBasic();
	CacheCBufferSkin();
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

bool jShaderSkin::OnRender(ObjCamera* cam)
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
	Draw(sizeof(VertexFormatPTNIW), prim, vertCount, mMesh->GetIndicies().size());
	return true;
}

void jShaderSkin::UpdateInputResource()
{
	LoadMesh();
	LoadCompentIndicies();
	
	mTextureViews.clear();
	LoadCompentTextures();
}

void jShaderSkin::LoadLayout()
{
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

	CacheInputLayout(polygonLayout, 5, ResName_Layout, ResName_Shader_Vertex);
}

void jShaderSkin::LoadMesh()
{
	mMesh = GetGameObject()->FindComponent<jMesh>();
	mMesh->Load();
	string key = mMesh->GetFullname();

	vector<VertexFormatPTNIW> vertices;
	vector<VertexFormat>& meshVert = mMesh->GetVerticies();
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
	CacheCBVertex(&vertices[0], sizeof(VertexFormatPTNIW) * vertices.size(), key);
}


