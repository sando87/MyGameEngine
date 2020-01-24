#include "jShaderSprite.h"
#include "jRenderer.h"
#include "jMesh.h"
#include "ObjCamera.h"
#include "jImage.h"

#define ResName_Layout "jShaderSprite.layout"
#define ResName_Shader_Vertex "sprite.vs"
#define ResName_Shader_Pixel "sprite.ps"


jShaderSprite::jShaderSprite()
{
}


jShaderSprite::~jShaderSprite()
{
}

void jShaderSprite::OnLoad()
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

bool jShaderSprite::OnRender(ObjCamera* cam)
{
	SetRenderContext(cam);

	D3D_PRIMITIVE_TOPOLOGY prim;
	if (mMesh->GetPrimitive() == PrimitiveMode::LineList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	u32 stride = sizeof(VertexFormatPT);
	u32 vertCount = mMesh->GetStream() ? mMesh->GetStream()->size() / stride : mMesh->GetVerticies().size();
	Draw(stride, prim, vertCount, mMesh->GetIndicies().size());
	return true;
}

void jShaderSprite::LoadLayout()
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
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	CacheInputLayout(polygonLayout, 2, ResName_Layout, ResName_Shader_Vertex);
}
void jShaderSprite::LoadMesh()
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
}
void jShaderSprite::LoadTexture()
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

