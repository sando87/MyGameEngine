#include "ObjDiablo.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderDiablo.h"
#include "jShaderColor.h"
#include "jShaderSkinned.h"
#include "jTexture.h"
#include "jShaderTexture.h"
#include "jGameObjectMgr.h"
#include "jUtils.h"
#include <sstream>
#include <iostream>
#include "jLog.h"

ObjDiablo::ObjDiablo()
{
	mShader = nullptr;
	mModel = nullptr;
	mCBMainCount = 0;
	mCBMains = nullptr;
}


ObjDiablo::~ObjDiablo()
{
	if (mShader != nullptr)
	{
		mShader->Release();
		delete mShader;
		mShader = nullptr;
	}
	if (mModel != nullptr)
	{
		mModel->Release();
		delete mModel;
		mModel = nullptr;
	}
}
bool SetDrawInfo(void *_this, char *_filename)
{
	ObjDiablo* pObj = (ObjDiablo*)_this;
	vector<string> rets;
	jUtils::Split(string(_filename), "_", rets);
	if (stoi(rets[0]) != jGameObjectMgr::GetInst().mFileIndex)
		return true;

	if (rets[1] == "Draw")
	{
		pObj->mDrawInfos.IndexCount = stoi(rets[2]);
		pObj->mDrawInfos.StartIndex = stoi(rets[3]);
		pObj->mDrawInfos.VertexOffset = stoi(rets[4]);
	}
	if (rets[1] == "Primitive")
	{
		pObj->mDrawInfos.primitive = stoi(rets[2]);
	}
	return true;
}
void ObjDiablo::OnStart()
{
	//printf("[vs]\n");
	//jUtils::ForEachFiles2(nullptr, "D:\\temp\\*_v.dump", [&](void* ptr, string name) {
	//	vector<string> rets;
	//	jUtils::Split(name, "_", rets);
	//	int num = stoi(rets[0]);
	//	char* pBuf = nullptr;
	//	int size = 0;
	//	jUtils::LoadFile("D:\\temp\\" + name, &size, &pBuf);
	//	MyResBase* pData = (MyResBase*)pBuf;
	//	printf("case RES_ID(0x%x, 0x%x): //%d\n", pData->crc, pData->totalSize, num);
	//	return true;
	//});

	//LoadCBMatrix();
	if (!mRenderIfno.Init(mFileIndex))
	{
		_printlog("fail to load [%d]\n", mFileIndex);
		DeleteFromMgr();
		return;
	}

	mModel = new jModel();
	mModel->LoadDiablo_ForTextureShader(&mRenderIfno);

	ID3D11ShaderResourceView* pIF = mRenderIfno.GetResShaderResourceView();
	mTexture = new jTexture();
	mTexture->SetShaderResourceView(pIF);

	mShader = new jShaderTexture();
	mShader->Initialize("./texture.vs", "./texture.ps");

	//mShader = new jShaderSkinned();
	//mShader->Initialize("./test.vs", "./test.ps");
}

void ObjDiablo::OnUpdate()
{
}

void ObjDiablo::OnDraw()
{
	Matrix4 mat = Matrix4().identity();
	//mat[12] = -50.0f;
	//mat[13] = -50.0f;
	//mat[14] = -70.0f;
	//mShader->SetParams(mModel, mat, &GetCamera(), false);
	//mShader->Render();

	static int drawIndx = 0;
	CBMatrix* animMats = GetCBMatrix(drawIndx);
	drawIndx++;

	vector<Matrix4> mats;
	for (int i = 0; i < 45; ++i)
	{
		if (animMats != nullptr)
		{
			Matrix4 curMat;
			curMat[0] = animMats->mat[i].a.x;
			curMat[1] = animMats->mat[i].b.x;
			curMat[2] = animMats->mat[i].c.x;
			curMat[3] = 0;

			curMat[4] = animMats->mat[i].a.y;
			curMat[5] = animMats->mat[i].b.y;
			curMat[6] = animMats->mat[i].c.y;
			curMat[7] = 0;

			curMat[8] = animMats->mat[i].a.z;
			curMat[9] = animMats->mat[i].b.z;
			curMat[10] = animMats->mat[i].c.z;
			curMat[11] = 0;

			curMat[12] = animMats->mat[i].a.w;
			curMat[13] = animMats->mat[i].b.w;
			curMat[14] = animMats->mat[i].c.w;
			curMat[15] = 1;

			mats.push_back(curMat);
		}
		else
		{
			mats.push_back(Matrix4().identity());
		}
	}

	//mShader->SetParams(mModel, mat, &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector4f(1, 1, -1, 0), mats);
	//mShader->Render();

	//ID3D11BlendState* state = mRenderIfno.GetResBlendState();
	//if (state != nullptr)
	//	jRenderer::GetInst().GetDeviceContext()->OMSetBlendState(state, mRenderIfno.mContext.bs_factor, mRenderIfno.mContext.bs_mask);
	
	mShader->SetParams(mModel, mat, mTexture);
	mShader->Render();
}

CBMatrix* ObjDiablo::GetCBMatrix(int _idx)
{
	if (mCBMains == nullptr)
		return nullptr;

	int index = _idx % mCBMainCount;
	int off = 2216 * index;
	MyRes_CreateBuffer* pData = (MyRes_CreateBuffer*)(mCBMains + off);
	return (CBMatrix*)pData->data;
}
bool ObjDiablo::LoadCBMatrix()
{
	int fileSize = 0;
	char* pBuf = nullptr;
	string filename = PATH_RESOURCE;
	filename += "CBMatrix.bin";
	jUtils::LoadFile(filename, &fileSize, &pBuf);
	mCBMainCount = fileSize / 2216;
	mCBMains = pBuf;
	return true;
}