#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include <list>
#include <utility>
#include <unordered_map>

#include "jFuncPtrList.h"
#include "jTypeDef.h"
#include "jUtils.h"

using namespace std;
class jGameObject;
class ObjCamera;
class ObjTerrainMgr;
struct MyResBase;

class jGameObjectMgr
{
public:
	static jGameObjectMgr& GetInst() { static jGameObjectMgr inst; return inst; }

private:
	jGameObjectMgr();
	~jGameObjectMgr();

	ObjCamera* mCamera;
	ObjTerrainMgr* mTerrainMgr;
	list<jGameObject*> mObjects;

	unordered_map<string, function<bool(void)>> mCoroutines;
	

public:
	bool Initialize();
	void Release();
	void RunObjects();


	bool AddGPURes(MyResBase* pData, void* pInterface);
	bool InitResources(int _idx);
	void AddResource(MyResBase* _res);
	MyResBase* GetResource(void* _res);

	void StartCoroutine(function<bool(void)> coroutine);
	void StartCoroutine(function<bool(void)> coroutine, string name);
	void StopCoroutine(string name);

	void AddGameObject(jGameObject* _obj) { mObjects.push_back(_obj); }
	ObjCamera& GetCamera() { return *mCamera; }
	ObjTerrainMgr& GetTerrain() { return *mTerrainMgr; }

	unordered_map<void *, MyResBase*> mResources;
	unordered_map<void *, pair<MyResBase*, void *>> mGPURes;
	int mFileIndex;
};

#endif