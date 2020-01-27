#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include "junks.h"
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
	bool Added(jGameObject* obj);
	template<typename Ty = jGameObject> Ty* FindGameObject();
	template<typename Ty = jGameObject> shared_ptr_array<Ty*> FindGameObjects();
	jGameObject* FindGameObject(string objectName);
	jGameObjects FindGameObjects(string objectName);
};

template<typename Ty>
inline Ty * jGameObjectMgr::FindGameObject()
{
	for (auto iter : mObjects)
	{
		Ty* pp = dynamic_cast<Ty*>(iter.second);
		if (pp)
			return pp;
	}
	return nullptr;
}

template<typename Ty>
inline shared_ptr_array<Ty*> jGameObjectMgr::FindGameObjects()
{
	shared_ptr_array<Ty*> rets;
	for (auto iter : mObjects)
	{
		Ty* pp = dynamic_cast<Ty*>(iter.second);
		if (pp)
			rets->push_back(pp);
	}
	return rets;
}

#endif