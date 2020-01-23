#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include "junks.h"
#include "jGrid.h"
#include "jCoroutine.h"

class jGameObject;
class ObjCamera;
class jCrash;


typedef shared_ptr_array<jGameObject*> jGameObjects;
class jGameObjectMgr
{
public:
	static jGameObjectMgr& GetInst() { static jGameObjectMgr inst; return inst; }

private:
	jGameObjectMgr();
	~jGameObjectMgr();

	list<jGameObject*> mNewObjects;
	unordered_multimap<string, jGameObject*> mObjects;
	jGrid<list<jCrash*>> mCrashGrid;
	jCoroutine mCoroutine;

	int tmpIdx;

public:
	bool Initialize();
	void Release();
	void RunObjects();


	void StartCoRoutine(string name, std::function<CorCmd(CorMember&, bool)> coroutine);
	void StartCoRoutine(string name, float time_ms, std::function<CorCmd(CorMember&, bool)> coroutine);
	void StartCoRoutine(string name, std::function<void(void)> task, std::function<CorCmd(CorMember&, bool)> coroutine);
	void StopCoRoutine(string name);

	jGameObject* RayCast(Vector3 pos, Vector3 dir);

	void AddGameObject(jGameObject* _obj);
	jGameObject* FindGameObject(string objectName);
	jGameObjects FindGameObjects(string objectName);
	jGrid<list<jCrash*>>* GetCrashGrid();
};

#endif