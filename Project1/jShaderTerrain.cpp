#include "jShaderTerrain.h"
#include "jGlobalStruct.h"
#include "jGameObjectMgr.h"
#include "jTexture.h"
#include "jModel.h"
#include "ObjCamera.h"


jShaderTerrain::jShaderTerrain()
{
}


jShaderTerrain::~jShaderTerrain()
{
	Release();
}

bool jShaderTerrain::Initialize(string _vsFilename, string _psFilename)
{
	//Shader Compile & CreateShader Interface
	ID3D11Device* pDev = jRenderer::GetInst().GetDevice();
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	ID3D10Blob* vertexShaderBuffer = CompileVertexShader(_vsFilename);
	ID3D10Blob* pixelShaderBuffer = CompilePixelShader(_psFilename);
	if (vertexShaderBuffer == nullptr || pixelShaderBuffer == nullptr)
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
	if (FAILED(pDev->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout)))
		return false;

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//CreateBufferInterface
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(pDev->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer)))
		return false;

	//CreateBufferInterface
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TexelVector);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(pDev->CreateBuffer(&matrixBufferDesc, NULL, &mTexelVectorBuffer)))
		return false;

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
	if (FAILED(pDev->CreateSamplerState(&samplerDesc, &mSampleState)))
		return false;

	return true;
}

void jShaderTerrain::Release()
{
	if (mLayout) mLayout->Release();
	if (mMatrixBuffer) mMatrixBuffer->Release();
	if (mTexelVectorBuffer) mTexelVectorBuffer->Release();
	if (mSampleState) mSampleState->Release();
	mLayout = nullptr;
	mMatrixBuffer = nullptr;
	mTexelVectorBuffer = nullptr;
	mSampleState = nullptr;
}

bool jShaderTerrain::Render()
{
	ObjCamera& cam = jGameObjectMgr::GetInst().GetCamera();
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();

	unsigned int stride = mModel->GetVertexTypeSize();
	ID3D11Buffer *pVertBuffer = mModel->GetVertexBuffer();
	ID3D11Buffer *pIndexBuffer = mModel->GetIndexBuffer();

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetVertexBuffers(0, 1, &pVertBuffer, &stride, (const UINT *)&mModel->mVertexOff_setting);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	if (mModel->IsIndiciesStrideTwo())
		pDevContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	else
		pDevContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	int primitive = mModel->mPrimitiveTopology == 0 ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : mModel->mPrimitiveTopology;
	pDevContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)primitive);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(pDevContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = Matrix4(mWorld).transpose();
	dataPtr->view = cam.getPosMat_D3D().transpose();
	dataPtr->projection = cam.getProjMat().transpose();
	pDevContext->Unmap(mMatrixBuffer, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	D3D11_MAPPED_SUBRESOURCE texelMapRes;
	if (FAILED(pDevContext->Map(mTexelVectorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &texelMapRes)))
	{
		return false;
	}
	dataPtr = (MatrixBufferType*)texelMapRes.pData;
	memcpy(dataPtr, mTexelVectors, sizeof(Vector4f) * 12);
	pDevContext->Unmap(mTexelVectorBuffer, 0);
	pDevContext->VSSetConstantBuffers(1, 1, &mTexelVectorBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	for (int i = 0; i < 6; ++i)
		pDevContext->PSSetShaderResources(i, 1, &mTexture[i]);
	

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	pDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	pDevContext->VSSetShader(mVertexShader, NULL, 0);
	pDevContext->PSSetShader(mPixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	pDevContext->PSSetSamplers(0, 1, &mSampleState);

	// �ﰢ���� �׸��ϴ�.
	pDevContext->DrawIndexed(mModel->GetIndexCount(), mModel->mStartIndex, mModel->mVertexOff);
	return true;
}

void jShaderTerrain::SetParams(jModel * _model, Matrix4 _worldMat, jTexture * _texture[6], Vector4f _texels[12])
{
	mModel = _model;
	for(int i = 0; i < 6; ++i)
		mTexture[i] = _texture[i]->mTextureView;
	mWorld = _worldMat;
	memcpy(mTexelVectors, _texels, sizeof(Vector4f) * 12);
}
