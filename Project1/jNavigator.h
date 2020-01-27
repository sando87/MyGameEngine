#pragma once
#include "jComponent.h"
#include "jAStar.h"

class ObjCamera;
class ObjTerrainMgr;
class jGameObject;

class jNavigator :
	public jComponent
{
public:
	jNavigator();
	virtual ~jNavigator();

	void StartNavigate(Vector2 destPos, function<void(list<Vector2>&)> waypoints);
protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	void OptimizeRouteResults(vector<Vector2>& inPoints, int count, Vector2 startPos, list<Vector2>& outPoints);

	jAStar mAstar;
	ObjCamera * mCamera;
	ObjTerrainMgr * mTerrain;
};

