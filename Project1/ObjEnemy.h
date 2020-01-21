#pragma once
#include "jGameObject.h"
#include "jStateMachine.h"

class jAnimCSV;
class jShaderSkin;
class jHealthPoint;

class ObjEnemy :
	public jGameObject
{
public:
	ObjEnemy();
	virtual ~ObjEnemy();

	virtual void OnStart();
	virtual void OnUpdate();

	bool DetectPlayerAround(double round);

private:
	jAnimCSV* mAnim;
	jShaderSkin* mShader;
	jHealthPoint* mHP;
	jGameObject* mPlayer;

	class StateMachEnemy : public jStateMachine {
	private:
		double mAccTime;
		Vector2 mPatrolPos;
		virtual void OnPatrol();
		virtual void OnChase();
		virtual void OnAttack();
	public:
		void InitState();
	};
	StateMachEnemy* mStateMach;
};

