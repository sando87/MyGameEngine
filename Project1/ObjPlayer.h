#pragma once
#include "jGameObject.h"
#include "junks.h"

class jShaderSkin;
class jAnimator;
class jAStar;
class ObjCamera;
class ObjTerrainMgr;

class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

private:
	virtual void OnStart();
	virtual void OnUpdate();

	Vector2 MoveTo(Vector2 pos);
	void StartNavigate(Vector2 pos);
	void FollowWayPoints();
	void GoToTarget();
	void OptimizeRouteResults(vector<Vector2>& inPoints, int startIdx, Vector2 startPos, list<Vector2>& outPoints);

	jShaderSkin* mShader;
	jAnimator* mAnim;
	jAStar* mAstar;
	jGameObject* mTarget;
	ObjCamera * mCamera;
	ObjTerrainMgr * mTerrain;

	list<Vector2> mWayPoints;
	float mAnimTime = 0;
};

