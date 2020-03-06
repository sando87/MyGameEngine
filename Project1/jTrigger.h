#pragma once
#include "jComponent.h"
class jTrigger :
	public jComponent
{
	virtual void OnTriggered(jGameObject* object) = 0;
};

