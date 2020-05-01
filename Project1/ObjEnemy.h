#pragma once
#include "jGameObject.h"

class jAnimator;
class jHealthPoint;
class ObjTerrainMgr;
class cMovement;
class cColliderCylinder;

class ObjEnemy :
	public jGameObject
{
public:
	ObjEnemy();
	virtual ~ObjEnemy();

	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

private:
	jHealthPoint* mHP;
	jAnimator* mAnim;
	cMovement* mMovement;
	cColliderCylinder* mCollider;

	double mAccTime = 0;
	double mNextActionTime = 0;

	void DoAction();
	void AttackObject(jGameObject* obj);
	jGameObject* IsAroundTarget(double round);
	void Death();
};

