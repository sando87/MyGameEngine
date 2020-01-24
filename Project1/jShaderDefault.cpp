#include "jShaderDefault.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jCaches.h"
#include "jRenderer.h"

#define ResName_Layout "jShaderDefault.layout"
#define ResName_Shader_Vertex "default.vs"
#define ResName_Shader_Pixel "default.ps"

jShaderDefault::jShaderDefault()
{
}


jShaderDefault::~jShaderDefault()
{
}

void jShaderDefault::OnLoad()
{
	CacheCBufferBasic();
	CacheSamplerState();
	CacheBlendState();
	CacheDepthState();
	CacheVS(ResName_Shader_Vertex);
	CachePS(ResName_Shader_Pixel);

	LoadLayout();
	LoadMesh();
	LoadTexture();
	
}

bool jShaderDefault::OnRender(ObjCamera* cam)
{
	SetRenderContext(cam);

	D3D_PRIMITIVE_TOPOLOGY prim;
	if (mMesh->GetPrimitive() == PrimitiveMode::LineList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	u32 stride = sizeof(VertexFormatPTN);
	u32 vertCount = mMesh->GetStream() ? mMesh->GetStream()->size() / stride : mMesh->GetVerticies().size();
	Draw(stride, prim, vertCount, mMesh->GetIndicies().size());
	return true;
}

void jShaderDefault::LoadLayout()
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	CacheInputLayout(polygonLayout, 3, ResName_Layout, ResName_Shader_Vertex);
}

void jShaderDefault::LoadMesh()
{
	mMesh = GetGameObject()->FindComponent<jMesh>();
	mMesh->Load();
	string key = mMesh->GetFullname();

	if (mMesh->GetStream())
	{
		chars stream = mMesh->GetStream();
		CacheCBVertex(&stream[0], stream->size(), key);
	}
	else
	{
		vector<VertexFormatPTN> vertices;
		vector<VertexFormat>& meshVert = mMesh->GetVerticies();
		int cnt = meshVert.size();
		for (int i = 0; i < cnt; ++i)
		{
			VertexFormatPTN vertex;
			vertex.p = meshVert[i].position;
			vertex.t = meshVert[i].texel;
			vertex.n = meshVert[i].normal;
			vertices.push_back(vertex);
		}
		CacheCBVertex(&vertices[0], sizeof(VertexFormatPTN) * vertices.size(), key);
	}
}

void jShaderDefault::LoadTexture()
{
	mImage = GetGameObject()->FindComponent<jImage>();
	if (mImage == nullptr)
		return;

	mImage->Load();
	string keyFullname = mImage->GetFullname();
	string ext = jUtils::GetFileExtension(keyFullname);
	bool compressed = ext == "dump" ? true : false;
	AddCachedTextureView(mImage->GetBuffer(), mImage->GetWidth(), mImage->GetHeight(), compressed, keyFullname);
}

