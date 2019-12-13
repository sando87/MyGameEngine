#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

#include "junks.h"

class jGameObjectMgr;
class ObjCamera;
class ObjTerrainMgr;
class jComponent;
class jMatrixControl;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject();
	virtual ~jGameObject();

	void AddToMgr();
	ObjCamera& GetCamera();
	ObjTerrainMgr& GetTerrain();
	void StartCoRoutine(std::function<bool(void)> coroutine, string name = "");
	void StopCoRoutine(string name);
	void DeleteFromMgr() { mIsRemoved = true; }
	jMatrixControl& GetTransport();

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

protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnDraw();

	bool mIsRemoved;
	bool mIsStarted;
	list<jComponent*> mComponents;
	jMatrixControl* mTransport;

};

#endif