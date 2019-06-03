#include "jShaderDiablo.h"
#include "jGlobalStruct.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jUtils.h"


jShaderDiablo::jShaderDiablo()
{
}


jShaderDiablo::~jShaderDiablo()
{
}


bool jShaderDiablo::Initialize(string _vsFilename, string _psFilename)
{
	ID3D11Device* pDev = jRenderer::GetInst().GetDevice();
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	{
		FILE *pFile = NULL;
		MyShader *pData = NULL;
		int filesize = 0;
		fopen_s(&pFile, "D:\\temp\\ShaderVert_0_0_0000023A71AD6478.dump", "rb");

		fseek(pFile, 0, SEEK_END);
		filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		pData = (MyShader*)malloc(filesize);
		fread_s(pData, filesize, filesize, 1, pFile);

		if (FAILED(pDev->CreateVertexShader(pData->data, filesize - 1, NULL, &mVertexShader)))
			return false;


		FILE *pFileLayout = NULL;
		MyLayout *pDataLayout = NULL;
		int filesizeLayout;
		fopen_s(&pFileLayout, "D:\\temp\\Layout_0_0_0000023AAABB17C8.dump", "rb");

		fseek(pFileLayout, 0, SEEK_END);
		filesizeLayout = ftell(pFileLayout);
		fseek(pFileLayout, 0, SEEK_SET);

		char names[260] = { 0, };
		pDataLayout = (MyLayout*)malloc(filesizeLayout);
		fread_s(pDataLayout, filesizeLayout, filesizeLayout, 1, pFileLayout);
		memcpy(names, pDataLayout->desc + pDataLayout->numEle, filesizeLayout - 2 - (sizeof(D3D11_INPUT_ELEMENT_DESC)*pDataLayout->numEle));

		string append = names;
		char dels[] = "_";
		vector<string> rets;
		jUtils::Split(append, dels, rets);
		for (int i = 0; i < rets.size(); ++i)
			pDataLayout->desc[i].SemanticName = rets[i].c_str();

		//pDataLayout->desc[0].SemanticName = "POSITION";
		//pDataLayout->desc[1].SemanticName = "NORMAL";
		//pDataLayout->desc[2].SemanticName = "COLOR";
		//pDataLayout->desc[3].SemanticName = "TANGENT";
		//pDataLayout->desc[4].SemanticName = "BINORMAL";
		//pDataLayout->desc[5].SemanticName = "TEXCOORD";
		//pDataLayout->desc[6].SemanticName = "TEXCOORD";

		if (FAILED(pDev->CreateInputLayout(pDataLayout->desc, pDataLayout->numEle, pData->data, filesize - 1, &mLayout)))
			return false;

		if (pData)
			free(pData);
		if (pDataLayout)
			free(pDataLayout);
		fclose(pFile);
		fclose(pFileLayout);
	}
	{
		FILE *pFile = NULL;
		MyShader *pData = NULL;
		int filesize = 0;
		fopen_s(&pFile, "D:\\temp\\ShaderPixel_0_0_0000023A71AD6DF8.dump", "rb");

		fseek(pFile, 0, SEEK_END);
		filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		pData = (MyShader*)malloc(filesize);
		fread_s(pData, filesize, filesize, 1, pFile);

		if (FAILED(pDev->CreatePixelShader(pData->data, filesize - 1, NULL, &mPixelShader)))
			return false;

		if (pData)
			free(pData);
		fclose(pFile);
	}
	{
		FILE *pFile = NULL;
		int filesize = 0;
		fopen_s(&pFile, "D:\\temp\\VSConstBuf_0000023A5B54C5E0.dump", "rb");

		fseek(pFile, 0, SEEK_END);
		filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		m_pData = (MyBuffer*)malloc(filesize);
		fread_s(m_pData, filesize, filesize, 1, pFile);
		CBMain* pMainBuffer = (CBMain*)&m_pData->data[0];

		// ��� ��� ������ ����ü�� �ۼ��մϴ�.
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(CBMain);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = m_pData->data;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		if (FAILED(pDev->CreateBuffer(&m_pData->desc, &indexData, &mMatrixBuffer)))
			return false;

		fclose(pFile);
	}

	return true;
}

void jShaderDiablo::Release()
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

void jShaderDiablo::SetParams(jModel * _model, Matrix4 _worldMat, ObjCamera * _camera, bool _isWire)
{
	mModel = _model;
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

bool jShaderDiablo::Render()
{
	ID3D11DeviceContext* pDevContext = jRenderer::GetInst().GetDeviceContext();
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int stride = mVertTypeSize;
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &stride, &offset);

	ID3D11Buffer * mVertBuf2 = mModel->GetVertexBuffer2();
	stride = 8;
	offset = 0;
	pDevContext->IASetVertexBuffers(1, 1, &mVertBuf2, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	pDevContext->IASetIndexBuffer(mIdxBuf, mIsIndexTwo ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	if (mIsWire)
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//if (FAILED(pDevContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	//{
	//	return false;
	//}
	//CBMain* dataPtr = (CBMain*)mappedResource.pData;
	//memcpy(dataPtr, m_pData->data, sizeof(CBMain));
	//dataPtr->matWorld = mWorld;
	//dataPtr->matWorldView = mView;
	//dataPtr->matWorldViewProj = mProj;
	//pDevContext->Unmap(mMatrixBuffer, 0);
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
