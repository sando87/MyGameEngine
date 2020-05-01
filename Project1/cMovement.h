#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jAStar.h"

class ObjCamera;
class oCollisionMgr;
class cCollider;
class jHealthPoint;

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
	cCollider * mCollider;
	jHealthPoint* mHP;
	jGameObject * mTarget;
	Vector2 mDestination;
	double mDetectRange;
	bool mMoving;
	list<Vector2> mWaypoints;
	jAStar mAstar;

	Vector2 NextDestPos();
	Vector2 GetDestination();
	double GetSpeed();
	double GetRound();

public:
	function<void(jGameObject*)> EventDetected;
	void Move(Vector2 destPos, double detectRange = 0);
	void Move(jGameObject* target, double detectRange);
	void Stop();
	void Navigate();

};

