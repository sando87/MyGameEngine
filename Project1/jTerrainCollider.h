#pragma once
#include "jComponent.h"

class ObjTerrainMgr;

class jTerrainCollider :
	public jComponent
{
public:
	jTerrainCollider(double round);
	virtual ~jTerrainCollider();

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	double mRound;
	ObjTerrainMgr * mTerrain;
	Vector2 mDetectPoints[8];
};

