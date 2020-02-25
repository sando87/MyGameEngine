#include "jShaderUI.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"

#define ResName_Layout "jShaderUI.layout"
#define ResName_Shader_Vertex "ui.vs"
#define ResName_Shader_Pixel "ui.ps"


jShaderUI::jShaderUI()
{
	mCameraUI = new ObjCamera();
}


jShaderUI::~jShaderUI()
{
	if (mCameraUI)
		delete mCameraUI;
}

void jShaderUI::OnLoad()
{
	u32 num = 1;
	D3D11_VIEWPORT viewPort;
	mDevContext->RSGetViewports(&num, &viewPort);
	mCameraUI->GetTransform().lookat(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0));
	mCameraUI->setOrthogonalMatrix(0, viewPort.Width, viewPort.Height, 0, 0, 1);

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

	SetDepthOn(false);
	SetAlphaOn(true);
}


void jShaderUI::LoadLayout()
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

	polygonLayout[2].SemanticName = "COLOR";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	CacheInputLayout(polygonLayout, 3, ResName_Layout, ResName_Shader_Vertex);
}

void jShaderUI::LoadMesh()
{
	mCBVertex = (ID3D11Buffer *)mGraphicResources->CacheResource("jMeshUI", [&](string _name) {
		ID3D11Buffer *vertBuf = nullptr;
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VertexFormatPTC) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		vertBuf = CreateConstBuffer(&desc, nullptr);
		return vertBuf;
	});
}


void jShaderUI::DrawRect(VertexFormatPTC verticies[4], void* textureView)
{
	if (textureView != nullptr)
	{
		ID3D11ShaderResourceView *view = (ID3D11ShaderResourceView *)textureView;
		mDevContext->PSSetShaderResources(0, 1, &view);
	}
	else if (!mTextureViews.empty())
	{
		mDevContext->PSSetShaderResources(0, 1, &mTextureViews[0]);
	}


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	mDevContext->Map(mCBVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VertexFormatPTC* dest = (VertexFormatPTC*)mappedResource.pData;
	for (int i = 0; i < 4; ++i)
		dest[i] = verticies[i];
	mDevContext->Unmap(mCBVertex, 0);
	mDevContext->Draw(4, 0);
}

void jShaderUI::ConfigRender()
{
	SetRenderContext(mCameraUI);

	u32 offset = 0;
	u32 stride = sizeof(VertexFormatPTC);
	mDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDevContext->IASetVertexBuffers(0, 1, &mCBVertex, &stride, &offset);
}
