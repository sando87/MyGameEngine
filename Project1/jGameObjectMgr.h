#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include "junks.h"
#include "jGrid.h"
#include "jCoroutine.h"

class jGameObject;
class ObjCamera;
class ObjTerrainMgr;
class jCrash;


typedef shared_ptr_array<jGameObject*> jGameObjects;
class jGameObjectMgr
{
public:
	static jGameObjectMgr& GetInst() { static jGameObjectMgr inst; return inst; }
	static vector<jGameObject*> mmvec;

private:
	jGameObjectMgr();
	~jGameObjectMgr();

	ObjCamera* mCamera;
	ObjTerrainMgr* mTerrainMgr;
	unordered_multimap<string, jGameObject*> mObjects;
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

	jGameObject* RayCast(Vector3 pos, Vector3 dir);

	void AddGameObject(jGameObject* _obj, string objectName);
	jGameObject* FindGameObject(string objectName);
	jGameObjects FindGameObjects(string objectName);
	ObjCamera& GetCamera() { return *mCamera; }
	ObjTerrainMgr& GetTerrain() { return *mTerrainMgr; }
};

#endif