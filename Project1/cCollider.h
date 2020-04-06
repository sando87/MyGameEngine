#pragma once
#include "jComponent.h"
class cCollider :
	public jComponent
{
public:
	Vector3 GetWorldPos() { return Vector3(); }

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();
};

