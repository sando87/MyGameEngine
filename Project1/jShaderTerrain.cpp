#include "jShaderTerrain.h"
#include "ObjCamera.h"
#include "jImage.h"
#include "jMesh.h"

#define SHADER_TERRAIN_FILENAME "./terrain"

jShaderTerrain::jShaderTerrain()
{
}


jShaderTerrain::~jShaderTerrain()
{
}


bool jShaderTerrain::OnLoad()
{
	bool ret = CreateShaderAndLayout();
	if (ret) ret = CreateBuffers();
	if (ret) ret = CreateSamplerState();
	if (ret) ret = CreateInputBuffer();
	if (ret) ret = CreateTextures();

	_warnif(!ret);
	return ret;
}

bool jShaderTerrain::OnRender()
{
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	mDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &mVertexStride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	//mDevContext->IASetIndexBuffer(mIdxBuf, DXGI_FORMAT_R32_UINT, 0);
	mDevContext->IASetPrimitiveTopology(GetPrimitiveTriList() ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	mDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	mDevContext->VSSetShader(mVertexShader, NULL, 0);
	mDevContext->PSSetShader(mPixelShader, NULL, 0);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(mDevContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferWVP* dataPtr = (ShaderBufferWVP*)mappedResource.pData;
	dataPtr->world = GetGameObject()->GetTransport().getMatrix().transpose();
	dataPtr->view = GetGameObject()->GetCamera().getPosMat_D3D().transpose();
	dataPtr->projection = GetGameObject()->GetCamera().getProjMat().transpose();
	mDevContext->Unmap(mMatrixBuffer, 0);
	mDevContext->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	D3D11_MAPPED_SUBRESOURCE texelMapRes;
	if (FAILED(mDevContext->Map(mTexelVectorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &texelMapRes)))
	{
		return false;
	}
	ShaderParamsTerrain* vecTexels = (ShaderParamsTerrain*)texelMapRes.pData;
	memcpy(vecTexels, &mParams.vectors[0], sizeof(Vector4f) * 12);
	mDevContext->Unmap(mTexelVectorBuffer, 0);
	mDevContext->VSSetConstantBuffers(1, 1, &mTexelVectorBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	for (int i = 0; i < mTextureViews.size(); ++i)
		mDevContext->PSSetShaderResources(i, 1, &mTextureViews[i]);
	
	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	mDevContext->PSSetSamplers(0, 1, &mSampleState);

	// �ﰢ���� �׸��ϴ�.
	//mDevContext->DrawIndexed(mIndexCount, 0, 0);
	mDevContext->Draw(mVertexCount, 0);
	return true;
}

bool jShaderTerrain::CreateTextures()
{
	auto images = GetGameObject()->FindComponents<jImage>();
	for (jImage* compImage : *images)
	{
		string ext = jUtils::GetFileExtension(compImage->GetFullName());
		if (ext == "dump")
		{
			ID3D11ShaderResourceView *texView = LoadDumpTexture(compImage->GetFullName());
			mTextureViews.push_back(texView);
			continue;
		}

		int width = compImage->GetWidth();
		int height = compImage->GetHeight();
		int bufSize = compImage->GetBufferSize();
		char* buf = compImage->GetBuffer();

		//�ؽ�ó�� ����ü�� �����մϴ�.
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Height = height;
		textureDesc.Width = width;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		// �� �ؽ�ó�� �����մϴ�.
		ID3D11Texture2D* texture = nullptr;
		if (FAILED(mDev->CreateTexture2D(&textureDesc, NULL, &texture)))
		{
			_echoS("failed create 2D texture");
			return false;
		}

		//  targa �̹��� �������� �ʺ� ����� �����մϴ�.
		UINT rowPitch = (width * 4) * sizeof(unsigned char);

		// targa �̹��� �����͸� �ؽ�ó�� �����մϴ�.
		mDevContext->UpdateSubresource(texture, 0, NULL, buf, rowPitch, 0);

		// ���̴� ���ҽ� �� ����ü�� �����մϴ�.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		ID3D11ShaderResourceView *textureView = nullptr;
		// �ؽ�ó�� ���̴� ���ҽ� �並 ����ϴ�.
		if (FAILED(mDev->CreateShaderResourceView(texture, &srvDesc, &textureView)))
		{
			_echoS("failed create 2D textureResourceView");
			return false;
		}

		// �� �ؽ�ó�� ���� �Ӹ��� �����մϴ�.
		mDevContext->GenerateMips(textureView);
		mTextureViews.push_back(textureView);

		texture->Release();
		texture = nullptr;
	}
	return true;
}
bool jShaderTerrain::CreateShaderAndLayout()
{
	ID3D10Blob* vertexShaderBuffer = CompileShader(SHADER_TERRAIN_FILENAME);
	if (vertexShaderBuffer == nullptr)
		return false;

	//CreateLayout Interface
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
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	if (FAILED(mDev->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout)))
		return false;

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	return true;
}
bool jShaderTerrain::CreateBuffers()
{
	//CreateBufferInterface
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShaderBufferWVP);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(mDev->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer)))
		return false;

	//CreateBufferInterface
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShaderParamsTerrain);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(mDev->CreateBuffer(&matrixBufferDesc, NULL, &mTexelVectorBuffer)))
		return false;


	return true;
}
bool jShaderTerrain::CreateSamplerState()
{
	//Create SamplerState
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(mDev->CreateSamplerState(&samplerDesc, &mSampleState)))
		return false;

	return true;
}
bool jShaderTerrain::CreateInputBuffer()
{
	jGameObject* gameObj = GetGameObject();
	jMesh* mesh = gameObj->FindComponent<jMesh>();
	if (mesh == nullptr)
		return false;

	if (mesh->GetStream())
	{
		chars stream = mesh->GetStream();
		mVertexStride = sizeof(VertexFormatPT);
		mVertexCount = stream->size() / mVertexStride;

		// ���� ���� ������ ����ü�� �����մϴ�.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = mVertexStride * mVertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = &stream[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// ���� ���� ���۸� ����ϴ�.
		if (FAILED(mDev->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertBuf)))
		{
			_echoS("failed create VB");
			return false;
		}

		return true;
	}

	vector<VertexFormat>& meshVert = mesh->GetVerticies();
	vector<VertexFormatPT> vertices;
	int cnt = meshVert.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormatPT vertex;
		vertex.p = meshVert[i].position;
		vertex.t = meshVert[i].texel;

		vertices.push_back(vertex);
	}

	mVertexStride = sizeof(VertexFormatPT);
	mVertexCount = vertices.size();

	// ���� ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = mVertexStride * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(mDev->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertBuf)))
	{
		_echoS("failed create VB");
		return false;
	}

	return true;
}

