#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

#include "junks.h"

class jGameObjectMgr;
class ObjCamera;
class ObjTerrainMgr;
class jComponent;
class jMatrixControl;
enum CoroutineReturn;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject();
	virtual ~jGameObject();

	void AddToMgr();
	ObjCamera& GetCamera();
	ObjTerrainMgr& GetTerrain();
	void StartCoRoutine(std::function<CoroutineReturn(void)> coroutine, string name = "");
	void StartCoRoutine(std::function<CoroutineReturn(void)> coroutine, float time_ms, string name = "");
	void StartCoRoutine(std::function<CoroutineReturn(void)> coroutine, std::function<void(void)> task, string name = "");
	void StopCoRoutine(string name);
	void DeleteFromMgr() { mIsRemoved = true; }
	jMatrixControl& GetTransport();
	bool LoadTxt(string objName);

	void AddComponent(jComponent* comp);
	void RemoveComponent(jComponent* comp);
	template<typename T> T* FindComponent()
	{
		for (jComponent* comp : mComponents)
		{
			T* pp = dynamic_cast<T*>(comp);
			if (pp)
				return pp;
		}
		return nullptr;
	}
	template<typename T> shared_ptr_array<T*> FindComponents()
	{
		shared_ptr_array<T*> rets;
		for (jComponent* comp : mComponents)
		{
			T* pp = dynamic_cast<T*>(comp);
			if (pp)
				rets->push_back(pp);
		}
		return rets;
	}

protected:
	virtual void OnStart();
	virtual void OnUpdate();

	bool mIsRemoved;
	bool mIsStarted;
	list<jComponent*> mComponents;
	jMatrixControl* mTransport;

};

#endif