#include "jShaderDiablo.h"
#include "jGlobalStruct.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jUtils.h"
#include "jGameObjectMgr.h"

jShaderDiablo::jShaderDiablo()
{
}


jShaderDiablo::~jShaderDiablo()
{
}

bool gluInvertMatrix(double* m, double* invOut)
{
	double inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
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

		//Matrix4 matW = pMainBuffer->matWorld.transpose();
		//Matrix4 matWV = pMainBuffer->matWorldView.transpose();
		//Matrix4 matWVP = pMainBuffer->matWorldViewProj.transpose();
		//Matrix4 matWInv = matW;
		//matWInv.invert();
		//Matrix4 matWVInv = matWV;
		//matWVInv.invert();
		//Matrix4 matV = matWInv * matWV;
		//Matrix4 matP = matWVInv * matWVP;
		//matW[12] -= 250.0f;
		//matW[13] -= 250.0f;
		//matW[14] = -60.0f;
		//Matrix4 newMatWVP = matW * matV * matP;
		//pMainBuffer->matWorldViewProj = newMatWVP.transpose();

		//Matrix4f matW = pMainBuffer->matWorld;
		//Matrix4f matV = pMainBuffer->matWorldView;
		//Matrix4f matP = pMainBuffer->matWorldViewProj;
		Matrix4 matW = Matrix4().identity();
		//matW[12] = 1200.0f;
		//matW[13] = 3800.0f;
		//matW[14] = 60.0f;
		Matrix4 matV = jGameObjectMgr::GetInst().GetCamera().getPosMat_D3D();
		Matrix4 matP = jGameObjectMgr::GetInst().GetCamera().getProjMat();
		Matrix4 matWVP = matW * matV * matP;
		pMainBuffer->matWorldViewProj = matWVP.transpose();

		// 행렬 상수 버퍼의 구조체를 작성합니다.
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

	// 레이아웃을 해제합니다.
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
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int stride = mVertTypeSize;
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	pDevContext->IASetVertexBuffers(0, 1, &mVertBuf, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	pDevContext->IASetIndexBuffer(mIdxBuf, mIsIndexTwo ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형으로 설정합니다.
	if (mIsWire)
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
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



	// 정점 입력 레이아웃을 설정합니다.
	pDevContext->IASetInputLayout(mLayout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	pDevContext->VSSetShader(mVertexShader, NULL, 0);
	pDevContext->PSSetShader(mPixelShader, NULL, 0);

	// 삼각형을 그립니다.
	pDevContext->DrawIndexed(mIndexCount, 0, 0);
	return true;
}
