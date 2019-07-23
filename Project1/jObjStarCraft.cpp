#include "jObjStarCraft.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderDiablo.h"
#include "jShaderColor.h"
#include "jShaderSkinned.h"
#include "jShaderTerrain.h"
#include "jTexture.h"
#include "jShaderTexture.h"
#include "jGameObjectMgr.h"
#include "jUtils.h"
#include <sstream>
#include <iostream>
#include "jLog.h"


jObjStarCraft::jObjStarCraft()
{
	mModel = nullptr;
	mShader = nullptr;
	mTexture = nullptr;

	mm_x = 0;
	mFileIndex = 0;
}


jObjStarCraft::~jObjStarCraft()
{
	if (mModel) mModel->Release();
	if (mShader) mShader->Release();
	if (mTexture) mTexture->Release();
}

void jObjStarCraft::OnStart()
{
	static float gg_x = 0;
	mm_x = gg_x;
	gg_x += 10.0f;

	if (!mRenderIfno.Init(mFileIndex))
	{
		_printlog("fail to load [%d]\n", mFileIndex);
		DeleteFromMgr();
		return;
	}

	//mRenderIfno.ExportToObjectFormat();

	mModel = new jModel();
	mModel->LoadDiablo_ForTextureShader(&mRenderIfno);

	ID3D11ShaderResourceView* pIF = mRenderIfno.GetResShaderResourceView(0);
	mTexture = new jTexture();
	mTexture->SetShaderResourceView(pIF);

	mShader = new jShaderTexture();
	mShader->Initialize("./texture.vs", "./texture.ps");

	//mShader = new jShaderSkinned();
	//mShader->Initialize("./test.vs", "./test.ps");
	_printlog("done OnStart [%d]\n", mFileIndex);
}

void jObjStarCraft::OnUpdate()
{
}
void jObjStarCraft::OnDraw()
{
	Matrix4 mat = Matrix4().identity();
	mat[12] = mm_x;

	mShader->SetParams(mModel, mat, mTexture);
	mShader->Render();

}
