#include "jShaderSkin.h"
#include "jMesh.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jCaches.h"
#include "jRenderer.h"

#define SHADER_SKIN_FILENAME "./test"

jShaderSkin::jShaderSkin()
{
	memset(&mParams, 0x00, sizeof(mParams));
}


jShaderSkin::~jShaderSkin()
{
}

bool jShaderSkin::OnLoad()
{
	bool ret = CreateShaderAndLayout();
	if (ret) ret = CreateBuffers();
	if (ret) ret = CreateSamplerState();
	if (ret) ret = CreateInputBuffer();
	if (ret) CreateTexture();

	_warnif(!ret);
	return ret;
}

bool jShaderSkin::OnRender()
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int offset = 0;
	
	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	mDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &mVertexStride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	//mDevContext->IASetIndexBuffer(mIdxBuf, DXGI_FORMAT_R32_UINT, 0);
	mDevContext->IASetPrimitiveTopology(GetPrimitiveTriList() ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
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
	ShaderBufferMatrixs* dataPtr = (ShaderBufferMatrixs*)mappedResource.pData;
	//Matrix4 worldMat = GetGameObject()->GetTransport().getMatrix();
	//Matrix4 retMat = worldMat * jLine3D::ProjMatGround(Vector3(-0.1, 0.1, -1), 0);
	//dataPtr->world = retMat.transpose();
	dataPtr->world = GetGameObject()->GetTransport().getMatrix().transpose();
	dataPtr->view = GetGameObject()->GetCamera().getPosMat_D3D().transpose();
	dataPtr->projection = GetGameObject()->GetCamera().getProjMat().transpose();
	for (int i = 0; i<45; ++i)
		dataPtr->bones[i] = mParams.bones[i];
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
	//mDevContext->DrawIndexed(mIndexCount, 0, 0);
	mDevContext->Draw(mVertexCount, 0);
	return true;
}

bool jShaderSkin::CreateShaderAndLayout()
{
	ID3D10Blob* vertexShaderBuffer = CompileShader(SHADER_SKIN_FILENAME);
	if (vertexShaderBuffer == nullptr)
		return false;

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
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
bool jShaderSkin::CreateBuffers()
{
	// 행렬 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShaderBufferMatrixs);
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
bool jShaderSkin::CreateSamplerState()
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
bool jShaderSkin::CreateTexture()
{
	jGameObject* gameObj = GetGameObject();
	jImage* compImage = gameObj->FindComponent<jImage>();
	if (compImage == nullptr)
		return false;

	string ext = jUtils::GetFileExtension(compImage->GetFullName());
	if (ext == "dump")
	{
		mTextureView = LoadDumpTexture(compImage->GetFullName());
		return true;
	}

	string key = compImage->GetFullName() + ".texture";
	mTextureView = (ID3D11ShaderResourceView *)jCaches::CacheGraphics(key, [this, compImage](string no_use) {
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
bool jShaderSkin::CreateInputBuffer()
{
	jGameObject* gameObj = GetGameObject();
	jMesh* mesh = gameObj->FindComponent<jMesh>();
	if (mesh == nullptr)
		return false;

	if (mesh->GetStream())
	{
		chars stream = mesh->GetStream();
		mVertexStride = sizeof(VertexFormatPTNIW);
		mVertexCount = stream->size() / mVertexStride;

		// 정적 정점 버퍼의 구조체를 설정합니다.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = mVertexStride * mVertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = &stream[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// 이제 정점 버퍼를 만듭니다.
		if (FAILED(mDev->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertBuf)))
		{
			_echoS("failed create VB");
			return false;
		}

		return true;
	}

	vector<VertexFormat>& meshVert = mesh->GetVerticies();
	vector<VertexFormatPTNIW> vertices;
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

	mVertexStride = sizeof(VertexFormatPTNIW);
	mVertexCount = vertices.size();

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = mVertexStride * mVertexCount;
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

	return true;
}
