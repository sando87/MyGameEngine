#pragma once
#include "jGameObject.h"
class ObjTerrain :
	public jGameObject
{
public:
	ObjTerrain();
	~ObjTerrain();
private:
	void OnStart();
	void OnUpdate();
	void OnDraw();
};

