#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"
#include "ObjTutorial1.h"
#include "jCamera.h"

jGameObjectMgr::jGameObjectMgr()
{
	mCamera = nullptr;
}


jGameObjectMgr::~jGameObjectMgr()
{
	Release();
}

bool jGameObjectMgr::Initialize()
{
	mCamera = new jCamera();
	mCamera->init(45, 640 / 480, 1.0, 1000.0);
	mCamera->lookat(Vector3(-100, 100, -100), Vector3(0, 0, 0), Vector3(0, 1, 0));

	ObjTutorial1* obj = new ObjTutorial1();
	obj->AddToMgr();
	return true;
}

void jGameObjectMgr::RunObjects()
{
	for (auto it = mObjects.begin(); it != mObjects.end(); )
	{
		jGameObject* obj = *it;
		if (obj->mIsRemoved)
		{
			delete obj;
			mObjects.erase(it++);
		}
		else
			++it;
	}

	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		if ((*it)->mIsStarted)
			continue;

		(*it)->OnStart();
		(*it)->mIsStarted = true;
	}

	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
		(*it)->OnUpdate();

	jRenderer::GetInst().BeginScene();
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
		(*it)->OnDraw();
	jRenderer::GetInst().EndScene();
}

void jGameObjectMgr::Release()
{
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = *it;
		delete obj;
		obj = nullptr;
	}
	mObjects.clear();

	if (mCamera != nullptr)
	{
		delete mCamera;
		mCamera = nullptr;
	}
}
