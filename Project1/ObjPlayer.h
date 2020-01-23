#pragma once
#include "jGameObject.h"
#include "junks.h"
#include "jAnimator.h"

class jAStar;
class ObjCamera;
class ObjTerrainMgr;

class jAnimatorGroup
{
public:
	void AddChild(jAnimator* anim);
	string GetAnimation();
	void SetAnimation(string name);
	void AddEvent(string name, float rate, function<void(void)> event);
protected:
	vector<jAnimator*> mChildAnimators;
};

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

	jAStar* mAstar;
	jGameObject* mTarget;
	ObjCamera * mCamera;
	ObjTerrainMgr * mTerrain;
	jAnimatorGroup* mAnim;

	list<Vector2> mWayPoints;
	float mAnimTime = 0;

};

