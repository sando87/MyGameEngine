#include "jShaderColor.h"
#include "jMesh.h"
#include "ObjCamera.h"
#include "jRenderer.h"

#define SHADER_COLOR_FILENAME "./color"

jShaderColor::jShaderColor()
{
	mLayout = nullptr;
	mMatrixBuffer = nullptr;
	mVertBuf = nullptr;

	mVertexStride = 0;
	mVertexCount = 0;
}

jShaderColor::~jShaderColor()
{
}


bool jShaderColor::OnLoad()
{
	bool ret = CreateShaderAndLayout();
	if (ret) ret = CreateBuffers();
	if (ret) ret = CreateInputBuffer();

	_warnif(!ret);
	return ret;
}

bool jShaderColor::OnRender()
{
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int stride = mVertexStride;
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	mDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	//mDevContext->IASetIndexBuffer(mIdxBuf, DXGI_FORMAT_R32_UINT, 0);
	mDevContext->IASetPrimitiveTopology(GetPrimitiveTriList() ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

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

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	mDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	mDevContext->VSSetShader(mVertexShader, NULL, 0);
	mDevContext->PSSetShader(mPixelShader, NULL, 0);

	ID3D11BlendState *bs = GetAlphaOn() ? jRenderer::GetInst().GetBS_AlphaOn() : jRenderer::GetInst().GetBS_AlphaOff();
	mDevContext->OMSetBlendState(bs, nullptr, 0xffffffff);

	ID3D11DepthStencilState *dss = GetDepthOn() ? jRenderer::GetInst().GetDSS_DepthOn() : jRenderer::GetInst().GetDSS_DepthOff();
	mDevContext->OMSetDepthStencilState(dss, 1);

	// �ﰢ���� �׸��ϴ�.
	mDevContext->Draw(mVertexCount, 0);
	return true;
}

bool jShaderColor::CreateShaderAndLayout()
{
	ID3D10Blob* vertexShaderBuffer = CompileShader(SHADER_COLOR_FILENAME);
	if (vertexShaderBuffer == nullptr)
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
	if (FAILED(mDev->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout)))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	return true;
}

bool jShaderColor::CreateInputBuffer()
{
	jGameObject* gameObj = GetGameObject();
	jMesh* mesh = gameObj->FindComponent<jMesh>();
	if (mesh == nullptr)
		return false;

	vector<VertexFormat>& meshVert = mesh->GetVerticies();

	vector<VertexFormatPC> vertices;
	int cnt = meshVert.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormatPC vertex;
		vertex.p = meshVert[i].position;
		vertex.c = meshVert[i].color;

		vertices.push_back(vertex);
	}

	mVertexStride = sizeof(VertexFormatPC);
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

bool jShaderColor::CreateBuffers()
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShaderBufferWVP);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	if (FAILED(mDev->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer)))
	{
		return false;
	}
	return true;
}

