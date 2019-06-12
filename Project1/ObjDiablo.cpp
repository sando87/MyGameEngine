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
	LoadCBMatrix();
	DoRenderingContext(0);

	mModel = new jModel();
	mModel->LoadDiablo_ForSkkinedShader(&mRenderingContext);

	void* pIF = jGameObjectMgr::GetInst().mGPURes[mRenderingContext.tex[0].addr].second;
	mTexture = new jTexture();
	mTexture->SetShaderResourceView((ID3D11ShaderResourceView *)pIF);

	//mShader = new jShaderTexture();
	//mShader->Initialize("./texture.vs", "./texture.ps");

	mShader = new jShaderSkinned();
	mShader->Initialize("./test.vs", "./test.ps");
	
	//mTexture = new jTexture();
	//mTexture->Initialize("./Texture2D_0_1_0000015D1A1E9468.tga");
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
	if (animMats != nullptr)
	{
		for (int i = 0; i < 45; ++i)
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
	}
	else
	{
		mats.push_back(Matrix4().identity());
	}

	mShader->SetParams(mModel, mat, &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector4f(1, 1, -1, 0), mats);
	mShader->mIndexCount = mRenderingContext.draw_IndexCount;
	mShader->mStartIndex = mRenderingContext.draw_StartIndex;
	mShader->mVertexOff = mRenderingContext.draw_BaseVertex;
	mShader->mOffVertexOff = (mRenderingContext.vb[0].offset[0] / 48) * 20;
	mShader->Render();

	//mShader->SetParams(mModel, mat, mTexture);
	//mShader->mIndexCount = mRenderingContext.draw_IndexCount;
	//mShader->mStartIndex = mRenderingContext.draw_StartIndex;
	//mShader->mVertexOff = mRenderingContext.draw_BaseVertex;
	//mShader->mOffVertexOff = (mRenderingContext.vb[0].offset[0] / 48) * 20;
	//mShader->Render();
}

void ObjDiablo::DoRenderingContext(int _index)
{
	string name = PATH_RESOURCE + to_string(_index) + "_RenderingContext.bin";
	int size = 0;
	char* pBuf = NULL;
	jUtils::LoadFile(name, &size, &pBuf);
	memcpy(&mRenderingContext, pBuf, size);
	mRenderingContext.CreateResources(_index);
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
DrawingInfo ObjDiablo::DoDrawingInfo()
{
	DrawingInfo ret = { 0, };
	unordered_map<void *, MyResBase*>& res = jGameObjectMgr::GetInst().mResources;
	for (auto iter = res.begin(); iter != res.end(); ++iter)
	{
		void* addr = iter->first;
		MyResBase* pData = iter->second;
		if (pData->type == MYRES_TYPE_CreateVS)
		{
			ret.vertexShader = pData;
		}
		else if (pData->type == MYRES_TYPE_CreatePS)
		{
			ret.pixelShader = pData;
		}
		else if (pData->type == MYRES_TYPE_CreateTex)
		{
			ret.texture[0] = pData;
		}
		else if (pData->type == MYRES_TYPE_CreateBuffer)
		{
			if (((MyRes_CreateBuffer*)pData)->desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER)
			{
				ret.indexBuffer = pData;
			}
			else if (((MyRes_CreateBuffer*)pData)->desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER)
			{
				ret.vertexBuffer[0] = pData;
			}
			else if (((MyRes_CreateBuffer*)pData)->desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER)
			{
				ret.ConstBuffer = pData;
			}
		}
		else if (pData->type == MYRES_TYPE_CreateLayout)
		{
			ret.layout = pData;
		}
		else if (pData->type == MYRES_TYPE_CreateSample)
		{
			ret.sampler[0] = pData;
		}
		else
		{
			_warn();
		}
	}

	return ret;
}
