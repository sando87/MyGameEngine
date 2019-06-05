#include "jShaderColor.h"
#include "jModel.h"
#include "ObjCamera.h"

jShaderColor::jShaderColor()
{
	mLayout = nullptr;
	mMatrixBuffer = nullptr;
	mVertBuf = nullptr;
	mIdxBuf = nullptr;
	mIndexCount = 0;
	mVertTypeSize = 0;
	mIsWire = false;
}

jShaderColor::~jShaderColor()
{
	Release();
}

bool jShaderColor::Initialize(string _vsFilename, string _psFilename)
{
	ID3D11Device* pDev = jRenderer::GetInst().GetDevice();
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	ID3D10Blob* vertexShaderBuffer = CompileVertexShader(_vsFilename);
	ID3D10Blob* pixelShaderBuffer = CompilePixelShader(_psFilename);
	if (vertexShaderBuffer == nullptr || pixelShaderBuffer == nullptr)
		return false;

	// ���� �Է� ���̾ƿ� ����ü�� �����մϴ�.
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ��մϴ�.
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

	return true;
}

void jShaderColor::Release()
{
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

void jShaderColor::SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, bool _isWire)
{
	mVertBuf = _model->GetVertexBuffer();;
	mIdxBuf = _model->GetIndexBuffer();
	mIndexCount = _model->GetIndexCount();
	mVertTypeSize = _model->GetVertexTypeSize();
	mIsIndexTwo = _model->IsIndiciesStrideTwo();

	mWorld = _worldMat.transpose();
	mView = _camera->getPosMat_D3D().transpose();
	mProj = _camera->getProjMat().transpose();
	mIsWire = _isWire;
}

bool jShaderColor::Render()
{
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int stride = mVertTypeSize;
	unsigned int offset = 0;

	ID3D11RasterizerState* state;
	ID3D11RasterizerState* stateNew;
	pDevContext->RSGetState(&state);
	D3D11_RASTERIZER_DESC desc;
	state->GetDesc(&desc);
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	jRenderer::GetInst().GetDevice()->CreateRasterizerState(&desc, &stateNew);
	pDevContext->RSSetState(stateNew);

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetIndexBuffer(mIdxBuf, mIsIndexTwo ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0); 

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	if(mIsWire)
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else
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
	pDevContext->Unmap(mMatrixBuffer, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	pDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	pDevContext->VSSetShader(mVertexShader, NULL, 0);
	pDevContext->PSSetShader(mPixelShader, NULL, 0);

	// �ﰢ���� �׸��ϴ�.
	pDevContext->DrawIndexed(mIndexCount, 0, 0);
	return true;
}

