#pragma once
#include "jComponent.h"

class ObjTerrainMgr;

class jTerrainCollider :
	public jComponent
{
public:
	jTerrainCollider();
	virtual ~jTerrainCollider();

	function<void(double height)> EventUnderTerrain;
	double GetHeight();


protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	ObjTerrainMgr * mTerrain;

	Property_GetSetter(bool, OnTerrain, true)
};

