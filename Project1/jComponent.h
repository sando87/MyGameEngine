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
	void SetFullname(string val) { mFullname = val; mLoaded = false; }

protected:
	friend jComponent* jGameObject::AddComponent(jComponent* comp);

	jGameObjectMgr* mEngine = nullptr;
	Property_Getter(jGameObject*, GameObject, nullptr)
	Property_GetSetter(bool, Enable, true)
	Property_GetSetter(bool, Loaded, false)
	Property_GetSetter(string, CompName, "")
	Property_Getter(string, Fullname, "")
};



