#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

#include "junks.h"

class jGameObjectMgr;
class jComponent;
class jTransform;
class jShader;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject(string name = "");
	virtual ~jGameObject();

	jGameObject * GetParent();
	list<jGameObject*>& GetChilds() { return mChilds; }
	jTransform& GetTransform();
	jGameObjectMgr& GetEngine();
	bool LoadTxt(string fullname);
	void AddComponent(jComponent* comp);
	template<typename T = jComponent> T* FindComponent();
	template<typename T = jComponent> vector<T*> FindComponents();
	jComponent* FindComponent(string componentName);
	vector<jComponent*> FindComponents(string componentName);
	jGameObject* FindGameObject(string objectName);
	void Destroy();

protected:
	virtual void OnLoad() {}
	virtual void OnStart() {}
	virtual void OnUpdate() {}

	void LoadAll();
	void StartAll();
	void UpdateAll();
	void GetShaderAll(vector<jShader*>& outs);
	
	void RemoveComponent(jComponent* comp);
	void LoadComponents();
	void StartComponents();
	void UpdateComponents();

	void AddChild(jGameObject* child);
	void SubChild(jGameObject* child);

	list<jComponent*> mComponents;
	jTransform* mTransform;
	jGameObjectMgr* mEngine;
	jGameObject* mParent;
	list<jGameObject*> mChilds;


	Property_GetSetter(bool, Enable, true)
	Property_Getter(bool, Remove, false)
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
	if (nullptr != mParent)
		return mParent->FindComponent<T>();
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