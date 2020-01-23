#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

#include "junks.h"

class jGameObjectMgr;
class jComponent;
class jMatrixControl;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject();
	virtual ~jGameObject();

	jMatrixControl& GetTransport();
	jGameObjectMgr& GetEngine();
	Matrix4 GetWorldMat();
	bool LoadTxt(string filename);
	void AddComponent(jComponent* comp);
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
	
	void RemoveComponent(jComponent* comp);
	void LoadComponents();
	void UpdateComponents();

	void StandOnTerrain();
	void AddChild(jGameObject* child);

	list<jComponent*> mComponents;
	jMatrixControl* mTransport;
	jGameObjectMgr* mEngine;
	jGameObject* mParent;
	vector<jGameObject*> mChilds;


	Property_GetSetter(bool, Remove, false)
	Property_Getter(bool, Started, false)
	Property_Getter(string, Name, "")
};

#endif