#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include "junks.h"
#include "jGrid.h"
#include "jCoroutine.h"

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
	jCoroutine mCoroutine;

	int tmpIdx;

public:
	bool Initialize();
	void Release();
	void RunObjects();
	void AddCrashs();

	void StartCoroutine(CoroutineInfo coroutineInfo);
	void StopCoroutine(string name);

	vector<jGameObject*> CheckCrash(Vector3 pos, Vector3 dir);

	void AddGameObject(jGameObject* _obj) { mObjects.push_back(_obj); }
	ObjCamera& GetCamera() { return *mCamera; }
	ObjTerrainMgr& GetTerrain() { return *mTerrainMgr; }
};

#endif