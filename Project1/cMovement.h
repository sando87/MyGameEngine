#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jRectangle2D.h"
#include "jAStar.h"

class ObjCamera;
class oCollisionMgr;
class cCollider;
class jHealthPoint;
class ObjTerrainMgr;

class cMovement :
	public jComponent
{
public:
	cMovement();
	~cMovement();

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	ObjCamera * mCamera;
	oCollisionMgr * mCollision;
	ObjTerrainMgr * mTerrain;
	cCollider * mCollider;
	jHealthPoint* mHP;
	jGameObject * mTarget;
	Vector2 mDestination;
	double mDetectRange;
	bool mMoving;
	list<Vector2> mWaypoints;
	jAStar mAstar;

	bool IsObstacle(Vector2 destPos);
	bool IsAccessable(jRectangle2D rect);
	Vector2 NextDestPos();
	Vector2 GetDestination();
	double GetSpeed();
	double GetRound();

public:
	function<void(jGameObject*)> EventDetected;
	void Move(Vector2 destPos, double detectRange = 0);
	void Move(jGameObject* target, double detectRange);
	void Stop();
	bool Navigate();
	bool IsMoving() { return mMoving; }

};

