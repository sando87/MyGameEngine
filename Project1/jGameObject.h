#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

#include "junks.h"

class jGameObjectMgr;
class ObjCamera;
class ObjTerrainMgr;
class jComponent;
class jMatrixControl;
enum CorCmd;
struct CorMember;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject();
	virtual ~jGameObject();

	void AddToMgr();
	ObjCamera& GetCamera();
	ObjTerrainMgr& GetTerrain();
	void StartCoRoutine(string name, std::function<CorCmd(CorMember&, bool)> coroutine);
	void StartCoRoutine(string name, float time_ms, std::function<CorCmd(CorMember&, bool)> coroutine);
	void StartCoRoutine(string name, std::function<void(void)> task, std::function<CorCmd(CorMember&, bool)> coroutine);
	void StopCoRoutine(string name);
	jMatrixControl& GetTransport();
	bool LoadTxt(string objName);
	void StandOnTerrain();

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

	bool mIsStarted;
	list<jComponent*> mComponents;
	jMatrixControl* mTransport;


	Property_GetSetter(bool, Remove)

};

#endif