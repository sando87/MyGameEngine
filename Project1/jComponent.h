#pragma once
#include "junks.h"
#include "jGameObject.h"

class jGameObjectMgr;

class jComponent
{
public:
	jComponent();
	virtual ~jComponent();

	virtual void OnLoad() {}
	virtual void OnStart() {}
	virtual void OnUpdate() {}

	void Load();
	jGameObjectMgr& GetEngine();

protected:
	friend void jGameObject::AddComponent(jComponent* comp);

	jGameObjectMgr* mEngine = nullptr;
	Property_Getter(jGameObject*, GameObject, nullptr)
	Property_GetSetter(bool, Enable, true)
	Property_GetSetter(bool, Loaded, false)
	Property_GetSetter(string, CompName, "")
	Property_GetSetter(string, Fullname, "")
};



