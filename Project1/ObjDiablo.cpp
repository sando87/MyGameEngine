#include "ObjDiablo.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderDiablo.h"
#include "jShaderColor.h"
#include "jShaderSkinned.h"
#include "jTexture.h"

ObjDiablo::ObjDiablo()
{
	mShader = nullptr;
	mModel = nullptr;
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

void ObjDiablo::OnStart()
{
	mModel = new jModel();
	mModel->LoadDiablo();

	//mShader = new jShaderColor();
	//mShader->Initialize("./color.vs", "./color.ps");

	mShader = new jShaderSkinned();
	mShader->Initialize("./test.vs", "./test.ps");
	
	mTexture = new jTexture();
	mTexture->Initialize("./Texture2D_0_1_0000015D1A1E9468.tga");
}

void ObjDiablo::OnUpdate()
{
}

void ObjDiablo::OnDraw()
{
	Matrix4 mat = Matrix4().identity();
	mat[12] = -50.0f;
	mat[13] = -50.0f;
	//mat[14] = -70.0f;
	//mShader->SetParams(mModel, mat, &GetCamera(), false);
	//mShader->Render();

	vector<Matrix4> mats;
	mats.push_back(mat);
	mShader->SetParams(mModel, mat, &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector4f(1, 1, -1, 0), mats);
	mShader->Render();
}
