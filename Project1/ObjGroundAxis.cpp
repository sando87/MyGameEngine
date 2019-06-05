#include "ObjGroundAxis.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderColor.h"

ObjGroundAxis::ObjGroundAxis()
{
	mShader = nullptr;
	mModel = nullptr;
	mModelX = nullptr;
	mWorldMat.identity();
}


ObjGroundAxis::~ObjGroundAxis()
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

		mModelX->Release();
		delete mModelX;
	}
}

void ObjGroundAxis::OnStart()
{
	mWorldMat.identity();

	mModel = new jModel();
	mModel->LoadGrid(-100, 100, 200, 200, 10);

	mModelX = new jModel();
	mModelX->LoadAxis(10);

	mShader = new jShaderColor();
	mShader->Initialize("./color.vs", "./color.ps");

}

void ObjGroundAxis::OnUpdate()
{

}

void ObjGroundAxis::OnDraw()
{
	mShader->SetParams(mModel, mWorldMat, &GetCamera(), true);
	mShader->Render();
	mShader->SetParams(mModelX, mWorldMat, &GetCamera(), true);
	mShader->Render();
}
