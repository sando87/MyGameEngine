#include "jShaderParser.h"
#include "jParserD3.h"
#include "ObjCamera.h"
#include "jMatrixControl.h"
#include "jCaches.h"
#include "jRenderer.h"

#define SHADER_PARSER_FILENAME "./parser"

jShaderParser::jShaderParser()
{
}

jShaderParser::~jShaderParser()
{
}

bool jShaderParser::Load(jParserD3 * parser)
{
	ID3D10Blob* vertexShaderBuffer = CompileShader(SHADER_PARSER_FILENAME);
	if (vertexShaderBuffer == nullptr)
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
	if (FAILED(mDev->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout)))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	// ��� ��� ������ ����ü�� �ۼ��մϴ�.
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

	// Material ��� ������ ����ü�� �ۼ��մϴ�.
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

	// Material ��� ������ ����ü�� �ۼ��մϴ�.
	// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� ByteWidth�� �׻� 16�� ��� �����ϸ� �׷��� ������ CreateBuffer�� �����մϴ�.
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
	if (FAILED(mDev->CreateSamplerState(&samplerDesc, &mSampleState)))
	{
		_echoS("failed create SamplerState");
		return false;
	}

	mTexture = parser->GetResShaderResourceView(0);
	mIndexBuffer = parser->GetResIndexBuffer();

	auto info = parser->GetGeometryInfo();
	int vertexCount = info.vertexTotalCount;
	mIASetVertexBuffersStride = sizeof(VertexFormatPTNIW);
	vector<VertexFormatPTNIW> verticies;
	Vector2f tex[10];
	for (int i = 0; i < vertexCount; ++i)
	{
		VertexFormatPTNIW vert;
		vert.p = parser->GetPos(i);
		int cnt = parser->GetTex(i, tex);
		vert.t = tex[0];
		vert.n = parser->GetNor(i);
		vert.i = parser->GetMatIdx(i);
		vert.w = parser->GetMatWeight(i);
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
		return false;

	mIASetVertexBuffersOff = (info.vertexVertexByteOffset / info.vertexStride) * mIASetVertexBuffersStride;
	mIndexStride = info.indiciesIndexUnit; // 2 or 4
	mDrawIndexed1 = info.drawIndexCount;
	mDrawIndexed2 = info.drawIndexOffset;
	mDrawIndexed3 = info.drawVertOffset;
	mPrimitiveType = info.primitiveType;

	for(int i = 0 ; i < 45; ++i)
		mBones[i] = Matrix4().identity();

	return true;
}
void jShaderParser::OnLoad()
{
}
bool jShaderParser::OnRender()
{
	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	mDevContext->IASetVertexBuffers(0, 1, &mVertBuffer, &mIASetVertexBuffersStride, (const UINT *)&mIASetVertexBuffersOff);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	if (mIndexStride == 2)
		mDevContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	else
		mDevContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	mDevContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)mPrimitiveType);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(mDevContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferMatrixs* dataPtr = (ShaderBufferMatrixs*)mappedResource.pData;
	dataPtr->world = GetGameObject()->GetTransport().getMatrix().transpose();
	dataPtr->view = GetGameObject()->GetCamera().getPosMat_D3D().transpose();
	dataPtr->projection = GetGameObject()->GetCamera().getProjMat().transpose();
	for (int i = 0; i<45; ++i)
		dataPtr->bones[i] = mBones[i];
	mDevContext->Unmap(mMatrixBuffer, 0);
	mDevContext->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	// Material constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if (FAILED(mDevContext->Map(mMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferMaterial* dataPtrMaterial = (ShaderBufferMaterial*)mappedResource.pData;
	dataPtrMaterial->diffuse = Vector4f(1,1,1,1);
	mDevContext->Unmap(mMaterialBuffer, 0);
	mDevContext->PSSetConstantBuffers(0, 1, &mMaterialBuffer);

	// light constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if (FAILED(mDevContext->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	ShaderBufferLight* dataPtrLight = (ShaderBufferLight*)mappedResource.pData;
	dataPtrLight->direction = Vector4f(-1, -1, -1, 0);
	mDevContext->Unmap(mLightBuffer, 0);
	mDevContext->PSSetConstantBuffers(1, 1, &mLightBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	mDevContext->PSSetShaderResources(0, 1, &mTexture);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	mDevContext->IASetInputLayout(mLayout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	mDevContext->VSSetShader(mVertexShader, NULL, 0);
	mDevContext->PSSetShader(mPixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	mDevContext->PSSetSamplers(0, 1, &mSampleState);

	ID3D11BlendState *bs = GetAlphaOn() ? jRenderer::GetInst().GetBS_AlphaOn() : jRenderer::GetInst().GetBS_AlphaOff();
	mDevContext->OMSetBlendState(bs, nullptr, 0xffffffff);

	ID3D11DepthStencilState *dss = GetDepthOn() ? jRenderer::GetInst().GetDSS_DepthOn() : jRenderer::GetInst().GetDSS_DepthOff();
	mDevContext->OMSetDepthStencilState(dss, 1);

	// �ﰢ���� �׸��ϴ�.
	mDevContext->DrawIndexed(mDrawIndexed1, mDrawIndexed2, mDrawIndexed3);
	return true;
}

