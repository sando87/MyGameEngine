#include "ObjTerrain.h"
#include "jLog.h"
#include "jModel.h"
#include "jTexture.h"
#include "jShaderTerrain.h"
#include "jGameObjectMgr.h"
#include "jUtils.h"

ObjTerrain::ObjTerrain()
{
	mModel = nullptr;
	mShader = nullptr;
	for(int i = 0; i < 6; ++i)
		mTexture[i] = nullptr;

	mm_x = 0;
	mFileIndex = 0;
}


ObjTerrain::~ObjTerrain()
{
	if (mModel)
		mModel->Release();
	if (mShader)
		mShader->Release();
	for (int i = 0; i < 6; ++i)
		if (mTexture[i])
			mTexture[i]->Release();
}


void ObjTerrain::OnStart()
{
	static float g_div_factor = 0;
	mm_x = g_div_factor;
	g_div_factor += 50.0f;

	if (!mRenderIfno.Init(mFileIndex))
	{
		_printlog("fail to load [%d]\n", mFileIndex);
		DeleteFromMgr();
		return;
	}

	if (!mRenderIfno.IsTerrain() || mRenderIfno.mTextures.size() < 6)
	{
		printf("[%d] This isn't Terrain Floor\n", mFileIndex);
		DeleteFromMgr();
		return;
	}

	_printlog("Terrain[%d]\n", mFileIndex);

	mRenderIfno.ExportToObjectFormat();

	mModel = new jModel();
	mModel->LoadDiablo_ForTerrain(&mRenderIfno);

	for (int i = 0; i < 6; ++i)
	{
		ID3D11ShaderResourceView* pIF = mRenderIfno.GetResShaderResourceView(i);
		mTexture[i] = new jTexture();
		mTexture[i]->SetShaderResourceView(pIF);
	}

	mShader = new jShaderTerrain();
	mShader->Initialize("./terrain.vs", "./terrain.ps");

	for (int i = 0; i < 6; ++i)
	{
		Matrix4f& matTex = mRenderIfno.mCBMain.matTex[i +1];
		mTexels[i * 2 + 0] = Vector4f(matTex[0], matTex[1], matTex[3], 0);
		mTexels[i * 2 + 1] = Vector4f(matTex[4], matTex[5], matTex[7], 0);
	}
}

void ObjTerrain::OnUpdate()
{

}

void ObjTerrain::OnDraw()
{
	Matrix4 mat = Matrix4().identity();

	Matrix4 CBmat = mRenderIfno.mCBMain.matWorld;
	CBmat.transpose();
	static map<Vector3f, int> mmTest;
	Vector3f key;
	key.x = CBmat[12];
	key.y = CBmat[13];
	key.z = CBmat[14];
	if (mmTest.find(key) == mmTest.end())
	{
		printf("[%d] x: %f, y: %f, z: %f\n", mFileIndex, CBmat[12], CBmat[13], CBmat[14]);
		mmTest[key] = 1;
	}

	//mat[12] = mm_x;
	mat[12] = CBmat[12];
	mat[13] = CBmat[13];
	mat[14] = CBmat[14];


	mShader->SetParams(mModel, mat, mTexture, mTexels);
	mShader->Render();
}
