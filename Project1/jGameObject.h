#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

#include "junks.h"

class jGameObjectMgr;
class jComponent;
class jTransform;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject();
	virtual ~jGameObject();

	jGameObject * GetParent();
	jTransform& GetTransform();
	jGameObjectMgr& GetEngine();
	bool LoadTxt(string filename);
	void AddComponent(jComponent* comp);
	template<typename T = jComponent> T* FindComponent();
	template<typename T = jComponent> vector<T*> FindComponents();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();
	
	void RemoveComponent(jComponent* comp);
	void LoadComponents();
	void UpdateComponents();

	void AddChild(jGameObject* child);

	list<jComponent*> mComponents;
	jTransform* mTransform;
	jGameObjectMgr* mEngine;
	jGameObject* mParent;
	vector<jGameObject*> mChilds;


	Property_GetSetter(bool, Remove, false)
	Property_Getter(string, Name, "")
};


template<typename T>
inline T * jGameObject::FindComponent()
{
	for (jComponent* comp : mComponents)
	{
		T* pp = dynamic_cast<T*>(comp);
		if (pp)
			return pp;
	}
	return nullptr;
}

template<typename T>
inline vector<T*> jGameObject::FindComponents()
{
	vector<T*> rets;
	for (jComponent* comp : mComponents)
	{
		T* pp = dynamic_cast<T*>(comp);
		if (pp)
			rets.push_back(pp);
	}
	return rets;
}

#endif