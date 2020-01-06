#include "jShaderDefault.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jCaches.h"
#include "jRenderer.h"

#define SHADER_DEFAULT_FILENAME "./default"

jShaderDefault::jShaderDefault()
{
	memset(&mParams, 0x00, sizeof(mParams));
}


jShaderDefault::~jShaderDefault()
{
}

bool jShaderDefault::OnLoad()
{
	bool ret = CreateShaderAndLayout();
	if (ret) ret = CreateBuffers();
	if (ret) ret = CreateSamplerState();
	if (ret) ret = CreateInputBuffer();
	if (ret) CreateTexture();

	_warnif(!ret);
	return ret;
}

bool jShaderDefault::OnRender()
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	mDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &mVertexStride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	mDevContext->IASetIndexBuffer(mIdxBuf, DXGI_FORMAT_R32_UINT, 0);

	// 정점 입력 레이아웃을 설정합니다.
	mDevContext->IASetInputLayout(mLayout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	mDevContext->VSSetShader(mVertexShader, NULL, 0);
	mDevContext->PSSetShader(mPixelShader, NULL, 0);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
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

	// Material constant buffer를 잠글 수 있도록 기록한다.
	if (FAILED(mDevContext->Map(mMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferMaterial* dataPtrMaterial = (ShaderBufferMaterial*)mappedResource.pData;
	*dataPtrMaterial = mParams.material;
	mDevContext->Unmap(mMaterialBuffer, 0);
	mDevContext->PSSetConstantBuffers(0, 1, &mMaterialBuffer);

	// light constant buffer를 잠글 수 있도록 기록한다.
	if (FAILED(mDevContext->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferLight* dataPtrLight = (ShaderBufferLight*)mappedResource.pData;
	*dataPtrLight = mParams.light;
	mDevContext->Unmap(mLightBuffer, 0);
	mDevContext->PSSetConstantBuffers(1, 1, &mLightBuffer);


	if (mTextureView != nullptr)
	{
		// 픽셀 셰이더에서 셰이더 텍스처 리소스를 설정합니다.
		mDevContext->PSSetShaderResources(0, 1, &mTextureView);
		// 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
		mDevContext->PSSetSamplers(0, 1, &mSampleState);
	}

	ID3D11BlendState *bs = GetAlphaOn() ? jRenderer::GetInst().GetBS_AlphaOn() : jRenderer::GetInst().GetBS_AlphaOff();
	mDevContext->OMSetBlendState(bs, nullptr, 0xffffffff);

	ID3D11DepthStencilState *dss = GetDepthOn() ? jRenderer::GetInst().GetDSS_DepthOn() : jRenderer::GetInst().GetDSS_DepthOff();
	mDevContext->OMSetDepthStencilState(dss, 1);

	// 삼각형을 그립니다.
	mDevContext->DrawIndexed(mIndexCount, 0, 0);
	return true;
}

bool jShaderDefault::CreateShaderAndLayout()
{
	ID3D10Blob* vertexShaderBuffer = CompileShader(SHADER_DEFAULT_FILENAME);
	if (vertexShaderBuffer == nullptr)
		return false;

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
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
	
	// 레이아웃의 요소 수를 가져옵니다.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	if (FAILED(mDev->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout)))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	return true;
}
bool jShaderDefault::CreateBuffers()
{
	// 행렬 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShaderBufferWVP);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(mDev->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer)))
	{
		_echoS("failed create Buffer");
		return false;
	}

	// Material 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC materialBufferDesc;
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(ShaderBufferMaterial);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;
	if (FAILED(mDev->CreateBuffer(&materialBufferDesc, NULL, &mMaterialBuffer)))
	{
		_echoS("failed create Buffer");
		return false;
	}

	// Material 상수 버퍼의 구조체를 작성합니다.
	// D3D11_BIND_CONSTANT_BUFFER를 사용하면 ByteWidth가 항상 16의 배수 여야하며 그렇지 않으면 CreateBuffer가 실패합니다.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(ShaderBufferLight);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	if (FAILED(mDev->CreateBuffer(&lightBufferDesc, NULL, &mLightBuffer)))
	{
		_echoS("failed create Buffer");
		return false;
	}

	return true;
}
bool jShaderDefault::CreateSamplerState()
{
	// 텍스처 샘플러 상태 구조체를 생성 및 설정합니다.
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

	// 텍스처 샘플러 상태를 만듭니다.
	if (FAILED(mDev->CreateSamplerState(&samplerDesc, &mSampleState)))
	{
		_echoS("failed create SamplerState");
		return false;
	}
	return true;
}
bool jShaderDefault::CreateTexture()
{
	jGameObject* gameObj = GetGameObject();
	jImage* compImage = gameObj->FindComponent<jImage>();
	if (compImage == nullptr)
		return false;

	string name = compImage->GetFileName() + ".texture";
	mTextureView = (ID3D11ShaderResourceView *)jCaches::CacheGraphics(name, [this, compImage](string name) {
		ID3D11ShaderResourceView * textureView = nullptr;

		int width = compImage->GetWidth();
		int height = compImage->GetHeight();
		int bufSize = compImage->GetBufferSize();
		char* buf = compImage->GetBuffer();

		//텍스처의 구조체를 설정합니다.
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

		// 빈 텍스처를 생성합니다.
		ID3D11Texture2D* texture = nullptr;
		if (FAILED(mDev->CreateTexture2D(&textureDesc, NULL, &texture)))
		{
			_echoS("failed create 2D texture");
			return textureView;
		}

		//  targa 이미지 데이터의 너비 사이즈를 설정합니다.
		UINT rowPitch = (width * 4) * sizeof(unsigned char);

		// targa 이미지 데이터를 텍스처에 복사합니다.
		mDevContext->UpdateSubresource(texture, 0, NULL, buf, rowPitch, 0);

		// 셰이더 리소스 뷰 구조체를 설정합니다.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// 텍스처의 셰이더 리소스 뷰를 만듭니다.
		if (FAILED(mDev->CreateShaderResourceView(texture, &srvDesc, &textureView)))
		{
			_echoS("failed create 2D textureResourceView");
			return textureView;
		}

		// 이 텍스처에 대해 밉맵을 생성합니다.
		mDevContext->GenerateMips(textureView);

		texture->Release();
		texture = nullptr;

		return textureView;
	});

	return true;
}
bool jShaderDefault::CreateInputBuffer()
{
	jGameObject* gameObj = GetGameObject();
	jMesh* mesh = gameObj->FindComponent<jMesh>();
	if (mesh == nullptr)
		return false;

	vector<VertexFormat>& meshVert = mesh->GetVerticies();
	vector<u32>& meshTri = mesh->GetIndicies();

	vector<VertexFormatPTN> vertices;
	int cnt = meshVert.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormatPTN vertex;
		vertex.p = meshVert[i].position;
		vertex.t = meshVert[i].texel;
		vertex.n = meshVert[i].normal;

		vertices.push_back(vertex);
	}

	mVertexStride = sizeof(VertexFormatPTN);
	mIndexCount = meshTri.size();

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = mVertexStride * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(mDev->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertBuf)))
	{
		_echoS("failed create VB");
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(u32) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &meshTri[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(mDev->CreateBuffer(&indexBufferDesc, &indexData, &mIdxBuf)))
	{
		_echoS("failed create IB");
		return false;
	}


	return true;
}
