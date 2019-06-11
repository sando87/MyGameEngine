#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"
#include "ObjTutorial1.h"
#include "ObjCamera.h"
#include "ObjGroundAxis.h"
#include "ObjDiablo.h"
#include "jUtils.h"
#include "jGlobalStruct.h"

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
	//ObjGroundAxis* obj = new ObjGroundAxis();
	//obj->AddToMgr();
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

bool LoadResources(void *_this, char *_filename)
{
	jGameObjectMgr* pObj = (jGameObjectMgr*)_this;
	vector<string> rets;
	jUtils::Split(string(_filename), "_", rets);
	if (stoi(rets[0]) != pObj->mFileIndex)
		return true;

	if (rets[1] == "texture")
	{
		int slot = stoi(rets[4]);
		string filename = PATH_RESOURCE + string(_filename);

		FILE *pFile = NULL;
		fopen_s(&pFile, filename.c_str(), "rb");
		if (pFile == NULL)
			return true;

		MyResBase head;
		fread_s(&head, sizeof(head), sizeof(head), 1, pFile);

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}

		string tgaName = PATH_RESOURCE + string(_filename) + ".tga";
		fopen_s(&pFile, tgaName.c_str(), "rb");
		if (pFile == NULL)
			return true;

		fseek(pFile, 0, SEEK_END);
		int filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		MyRes_CreateTexture *pData = (MyRes_CreateTexture *)malloc(sizeof(MyRes_CreateTexture) + filesize);
		memset(pData, 0x00, sizeof(MyRes_CreateTexture) + filesize);
		fread_s(pData->data, filesize, filesize, 1, pFile);
		pData->head = head;
		pData->head.totalSize = sizeof(MyRes_CreateTexture) + filesize;
		pData->head.crc = slot;

		pObj->AddResource(&pData->head);

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}
	else
	{
		string path = PATH_RESOURCE;
		path += _filename;

		FILE *pFile = NULL;
		fopen_s(&pFile, path.c_str(), "rb");
		if (pFile == NULL)
			return true;

		fseek(pFile, 0, SEEK_END);
		int filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		MyResBase *pData = (MyResBase*)malloc(filesize);
		fread_s(pData, filesize, filesize, 1, pFile);
		char ch = ((char*)pData)[1];
		
		if(ch == 0)
			pObj->AddResource(pData);
		else
		{
			free(pData);
			pData = NULL;
		}

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}

	return true;
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
bool jGameObjectMgr::AddGPURes(MyResBase* pData, void* pInterface)
{
	mGPURes[pData->addr] = make_pair(pData, pInterface);
	return true;
}
bool jGameObjectMgr::InitResources(int _idx)
{
	string path = PATH_RESOURCE;
	path += "*.dump";
	mFileIndex = _idx;
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
