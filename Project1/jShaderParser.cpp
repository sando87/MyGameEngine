#include "jShaderParser.h"
#include "jParserD3.h"
#include "ObjCamera.h"
#include "jMatrixControl.h"

#define SHADER_PARSER_FILENAME "./texture"

jShaderParser::jShaderParser()
{
}

jShaderParser::~jShaderParser()
{
}

bool jShaderParser::LoadRenderResources()
{
	if (mLoaded)
		return true;

	ID3D10Blob* vertexShaderBuffer = CompileShader(SHADER_PARSER_FILENAME);
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
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
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

	mLoaded = true;
	return true;
}
void jShaderParser::Load(jParserD3 * parser)
{
	mTexture = parser->GetResShaderResourceView(0);
	mIndexBuffer = parser->GetResIndexBuffer();

	auto info = parser->GetGeometryInfo();
	int vertexCount = info.vertexTotalCount;
	mIASetVertexBuffersStride = sizeof(VertexFormatPT);
	vector<VertexFormatPT> verticies;
	Vector2f tex[10];
	for (int i = 0; i < vertexCount; ++i)
	{
		VertexFormatPT vert;
		vert.p = parser->GetPos(i);
		int cnt = parser->GetTex(i, tex);
		vert.t = tex[0];
		verticies.push_back(vert);
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = mIASetVertexBuffersStride * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &verticies[0];
	subRes.SysMemPitch = 0;
	subRes.SysMemSlicePitch = 0;
	if (FAILED(mDev->CreateBuffer(&vertexBufferDesc, &subRes, &mVertBuffer)))
		return;

	mIASetVertexBuffersOff = (info.vertexVertexByteOffset / info.vertexStride) * mIASetVertexBuffersStride;
	mIndexStride = info.indiciesIndexUnit; // 2 or 4
	mPrimitiveTopology = info.primitiveType;
	mDrawIndexed1 = info.drawIndexCount;
	mDrawIndexed2 = info.drawIndexOffset;
	mDrawIndexed3 = info.drawVertOffset;
}
bool jShaderParser::Render()
{
	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	mDevContext->IASetVertexBuffers(0, 1, &mVertBuffer, &mIASetVertexBuffersStride, (const UINT *)&mIASetVertexBuffersOff);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	if (mIndexStride == 2)
		mDevContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	else
		mDevContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	int primitive = mPrimitiveTopology == 0 ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : mPrimitiveTopology;
	mDevContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)primitive);

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

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	mDevContext->PSSetShaderResources(0, 1, &mTexture);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	mDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	mDevContext->VSSetShader(mVertexShader, NULL, 0);
	mDevContext->PSSetShader(mPixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	mDevContext->PSSetSamplers(0, 1, &mSampleState);

	// �ﰢ���� �׸��ϴ�.
	mDevContext->DrawIndexed(mDrawIndexed1, mDrawIndexed2, mDrawIndexed3);
	return true;
}

