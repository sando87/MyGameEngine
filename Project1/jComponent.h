#pragma once
#include "junks.h"
#include "jGameObject.h"

class jComponent
{
public:
	jComponent();
	virtual ~jComponent();

	virtual void OnLoad() {}
	virtual void OnUpdate() {}

protected:
	friend void jGameObject::AddComponent(jComponent* comp);

	Property_Getter(jGameObject*, GameObject, nullptr)
	Property_GetSetter(bool, Enable, true)
	Property_GetSetter(string, Fullname, "")
};



