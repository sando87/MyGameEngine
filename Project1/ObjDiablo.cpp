#include "ObjDiablo.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderDiablo.h"

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

	mShader = new jShaderDiablo();
	mShader->Initialize("./color.vs", "./color.ps");
}

void ObjDiablo::OnUpdate()
{
}

void ObjDiablo::OnDraw()
{
	mShader->SetParams(mModel, Matrix4().identity(), &GetCamera(), false);
	mShader->Render();
}
