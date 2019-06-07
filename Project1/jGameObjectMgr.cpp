#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"
#include "ObjTutorial1.h"
#include "ObjCamera.h"
#include "ObjGroundAxis.h"
#include "ObjDiablo.h"
#include "jUtils.h"
#include "jGlobalStruct.h"

#define PATH_RESOURCE "D:\\last\\"

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
	InitResources();

	mCamera = new ObjCamera();
	mCamera->AddToMgr();
	ObjGroundAxis* obj = new ObjGroundAxis();
	obj->AddToMgr();
	//ObjTutorial1* obj2 = new ObjTutorial1();
	//obj2->AddToMgr();
	ObjDiablo* obj3 = new ObjDiablo();
	obj3->AddToMgr();
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

void LoadResources(void *_this, char *_filename)
{
	jGameObjectMgr* pObj = (jGameObjectMgr*)_this;

	string path = PATH_RESOURCE;
	path += _filename;

	FILE *pFile = NULL;
	fopen_s(&pFile, path.c_str(), "rb");
	if (pFile == NULL)
		return;

	fseek(pFile, 0, SEEK_END);
	int filesize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	MyResBase *pData = (MyResBase*)malloc(filesize);
	fread_s(pData, filesize, filesize, 1, pFile);

	pObj->AddResource(pData);

	if (pFile)
		fclose(pFile);
}
void jGameObjectMgr::AddResource(MyResBase* _res)
{
	mResources[_res->addr] = _res;
}
MyResBase* jGameObjectMgr::GetResource(void* _res)
{
	if (mResources.find(_res) == mResources.end())
		return NULL;
	return mResources[_res];
}
bool jGameObjectMgr::InitResources()
{
	string path = PATH_RESOURCE;
	path += "*.dump";
	jUtils::ForEachFiles(this, path.c_str(), LoadResources);
	return true;
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

	for (auto it = mResources.begin(); it != mResources.end(); ++it)
	{
		MyResBase* obj = it->second;
		free(obj);
		obj = nullptr;
	}
	mResources.clear();
}
