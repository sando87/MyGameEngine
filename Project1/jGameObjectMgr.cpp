#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"
#include "ObjTutorial1.h"
#include "ObjCamera.h"
#include "ObjGroundAxis.h"

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
	mCamera = new ObjCamera();
	mCamera->AddToMgr();
	ObjGroundAxis* obj = new ObjGroundAxis();
	obj->AddToMgr();
	ObjTutorial1* obj2 = new ObjTutorial1();
	obj2->AddToMgr();
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
}
