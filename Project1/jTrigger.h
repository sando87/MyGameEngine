#pragma once
#include "jComponent.h"
class jTrigger :
	public jComponent
{
public:
	virtual void OnTriggered(jGameObject* object) = 0;
};

