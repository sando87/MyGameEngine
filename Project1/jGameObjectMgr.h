#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include "junks.h"
#include "jGrid.h"

class jGameObject;
class ObjCamera;
class ObjTerrainMgr;
class jCrash;

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
	jGrid<list<jCrash*>> mCrashGrid;
	unordered_map<string, function<bool(void)>> mCoroutines;
	

public:
	bool Initialize();
	void Release();
	void RunObjects();
	void AddCrashs();

	void StartCoroutine(function<bool(void)> coroutine);
	void StartCoroutine(function<bool(void)> coroutine, string name);
	void StopCoroutine(string name);

	void AddGameObject(jGameObject* _obj) { mObjects.push_back(_obj); }
	ObjCamera& GetCamera() { return *mCamera; }
	ObjTerrainMgr& GetTerrain() { return *mTerrainMgr; }
};

#endif