#include "ObjGroundAxis.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderColor.h"

ObjGroundAxis::ObjGroundAxis()
{
	mShader = nullptr;
	mModel = nullptr;
	mModelX = nullptr;
	mModelY = nullptr;
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
		mModelY->Release();
		mModelZ->Release();
		delete mModelX;
		delete mModelY;
		delete mModelZ;
	}
}

void ObjGroundAxis::OnStart()
{
	mWorldMat.identity();

	mModel = new jModel();
	mModel->LoadGrid(-100, 100, 200, 200, 10);

	mModelX = new jModel();
	mModelX->LoadAxisX(100);
	mModelY = new jModel();
	mModelY->LoadAxisY(100);
	mModelZ = new jModel();
	mModelZ->LoadAxisZ(100);

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
	mShader->SetParams(mModelY, mWorldMat, &GetCamera(), true);
	mShader->Render();
	mShader->SetParams(mModelZ, mWorldMat, &GetCamera(), true);
	mShader->Render();
}
