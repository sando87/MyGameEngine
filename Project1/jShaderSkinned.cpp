#include "jShaderSkinned.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jTexture.h"

jShaderSkinned::jShaderSkinned()
{
	mLayout = nullptr;
	mMatrixBuffer = nullptr;
	mMaterialBuffer = nullptr;
	mBoneMatBuffer = nullptr;
	mLightBuffer = nullptr;
	mVertBuf = nullptr;
	mIdxBuf = nullptr;
	mIndexCount = 0;
	mVertTypeSize = 0;
}

jShaderSkinned::~jShaderSkinned()
{
	Release();
}

bool jShaderSkinned::Initialize(string _vsFilename, string _psFilename)
{
	ID3D11Device* pDev = jRenderer::GetInst().GetDevice();
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	ID3D10Blob* vertexShaderBuffer = CompileVertexShader(_vsFilename);
	ID3D10Blob* pixelShaderBuffer = CompilePixelShader(_psFilename);
	if (vertexShaderBuffer == nullptr || pixelShaderBuffer == nullptr)
		return false;

	// ���� �Է� ���̾ƿ� ����ü�� �����մϴ�.
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ��մϴ�.
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

	// ���̾ƿ��� ��� ���� �����ɴϴ�.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� ����ϴ�.
	if (FAILED(pDev->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout)))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// ��� ��� ������ ����ü�� �ۼ��մϴ�.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(pDev->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer)))
	{
		return false;
	}

	// Bones ��� ������ ����ü�� �ۼ��մϴ�.
	// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� ByteWidth�� �׻� 16�� ��� �����ϸ� �׷��� ������ CreateBuffer�� �����մϴ�.
	//D3D11_BUFFER_DESC boneBufferDesc;
	//boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//boneBufferDesc.ByteWidth = sizeof(Matrix4f) * 4;
	//boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//boneBufferDesc.MiscFlags = 0;
	//boneBufferDesc.StructureByteStride = 0;
	//if (FAILED(pDev->CreateBuffer(&boneBufferDesc, NULL, &mBoneMatBuffer)))
	//{
	//	return false;
	//}

	// Material ��� ������ ����ü�� �ۼ��մϴ�.
	D3D11_BUFFER_DESC materialBufferDesc;
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBufferType);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;
	if (FAILED(pDev->CreateBuffer(&materialBufferDesc, NULL, &mMaterialBuffer)))
	{
		return false;
	}

	// Material ��� ������ ����ü�� �ۼ��մϴ�.
	// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� ByteWidth�� �׻� 16�� ��� �����ϸ� �׷��� ������ CreateBuffer�� �����մϴ�.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	if (FAILED(pDev->CreateBuffer(&lightBufferDesc, NULL, &mLightBuffer)))
	{
		return false;
	}

	// �ؽ�ó ���÷� ���� ����ü�� ���� �� �����մϴ�.
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

	// �ؽ�ó ���÷� ���¸� ����ϴ�.
	if (FAILED(pDev->CreateSamplerState(&samplerDesc, &mSampleState)))
	{
		return false;
	}

	return true;
}

void jShaderSkinned::Release()
{
	// ���÷� ���¸� �����Ѵ�.
	if (mSampleState)
	{
		mSampleState->Release();
		mSampleState = nullptr;
	}

	if (mBoneMatBuffer)
	{
		mBoneMatBuffer->Release();
		mBoneMatBuffer = nullptr;
	}
	if (mMaterialBuffer)
	{
		mMaterialBuffer->Release();
		mMaterialBuffer = nullptr;
	}
	if (mLightBuffer)
	{
		mLightBuffer->Release();
		mLightBuffer = nullptr;
	}
	if (mMatrixBuffer)
	{
		mMatrixBuffer->Release();
		mMatrixBuffer = nullptr;
	}

	// ���̾ƿ��� �����մϴ�.
	if (mLayout)
	{
		mLayout->Release();
		mLayout = nullptr;
	}

	jBaseShader::Release();

	mLayout = nullptr;
	mMatrixBuffer = nullptr;
	mVertBuf = nullptr;
	mIdxBuf = nullptr;
	mIndexCount = 0;
	mVertTypeSize = 0;
	mWorld.identity();
	mView.identity();
	mProj.identity();
}

void jShaderSkinned::SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, jTexture * _texture, Vector4f _diffuse, Vector4f _light, vector<Matrix4>& _mats)
{
	mTexture = _texture->mTextureView;
	mVertBuf = _model->GetVertexBuffer();;
	mIdxBuf = _model->GetIndexBuffer();
	mIndexCount = _model->GetIndexCount();
	mVertTypeSize = _model->GetVertexTypeSize();

	mWorld = _worldMat.transpose();
	mView = _camera->getPosMat().transpose();
	mProj = _camera->getProjMat().transpose();

	mDiffuse = _diffuse;
	mLight = _light;
	
	for(int i=0; i<_mats.size(); ++i)
		mBoneMats[i] = _mats[i];
}

bool jShaderSkinned::Render()
{
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int stride = mVertTypeSize;
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetIndexBuffer(mIdxBuf, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(pDevContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = mWorld;
	dataPtr->view = mView;
	dataPtr->projection = mProj;
	for (int i = 0; i<43; ++i)
		dataPtr->bones[i] = mBoneMats[i];
	pDevContext->Unmap(mMatrixBuffer, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	// bones constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	//memset(&mappedResource, 0x00, sizeof(mappedResource));
	//if (FAILED(pDevContext->Map(mBoneMatBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	//{
	//	return false;
	//}
	//memcpy(mappedResource.pData, mBoneMats, sizeof(Matrix4f) * 4);
	//pDevContext->Unmap(mBoneMatBuffer, 0);
	//pDevContext->VSSetConstantBuffers(1, 2, &mBoneMatBuffer);

	// Material constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if (FAILED(pDevContext->Map(mMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	MaterialBufferType* dataPtrMaterial = (MaterialBufferType*)mappedResource.pData;
	dataPtrMaterial->ambient = mDiffuse;
	dataPtrMaterial->diffuse = mDiffuse;
	dataPtrMaterial->specular = mDiffuse;
	dataPtrMaterial->shininess = mDiffuse;
	pDevContext->Unmap(mMaterialBuffer, 0);
	pDevContext->PSSetConstantBuffers(0, 2, &mMaterialBuffer);

	// light constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if (FAILED(pDevContext->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	LightBufferType* dataPtrLight = (LightBufferType*)mappedResource.pData;
	dataPtrLight->position = mLight;
	dataPtrLight->direction = mLight;
	dataPtrLight->color = mLight;
	dataPtrLight->reserve = mLight;
	pDevContext->Unmap(mLightBuffer, 0);
	pDevContext->PSSetConstantBuffers(1, 2, &mLightBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	pDevContext->PSSetShaderResources(0, 1, &mTexture);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	pDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	pDevContext->VSSetShader(mVertexShader, NULL, 0);
	pDevContext->PSSetShader(mPixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	pDevContext->PSSetSamplers(0, 1, &mSampleState);

	// �ﰢ���� �׸��ϴ�.
	pDevContext->DrawIndexed(mIndexCount, 0, 0);
	return true;
}

