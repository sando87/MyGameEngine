#pragma once
#include "jGameObject.h"
#include "jStateMachine.h"

class jAnimator;
class jHealthPoint;
class ObjTerrainMgr;

class ObjEnemy :
	public jGameObject
{
public:
	ObjEnemy();
	virtual ~ObjEnemy();

	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	bool DetectPlayerAround(double round);

private:
	jAnimator* mAnim;
	jHealthPoint* mHP;
	jGameObject* mPlayer;
	ObjTerrainMgr * mTerrain;

	class StateMachEnemy : public jStateMachine {
	private:
		double mAccTime;
		Vector2 mPatrolPos;
		ObjEnemy* mObject;
		virtual void OnLoad();
		virtual void OnPatrol();
		virtual void OnChase();
		virtual void OnAttack();
	};
	StateMachEnemy* mStateMach;
};

