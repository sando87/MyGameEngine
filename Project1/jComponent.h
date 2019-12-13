#pragma once
#include "junks.h"
#include "jGameObject.h"

class jComponent
{
public:
	jComponent();
	virtual ~jComponent();

	friend void jGameObject::AddComponent(jComponent* comp);

	Property_Getter(jGameObject*, GameObject)
	Property_GetSetter(bool, Enabled)
};



