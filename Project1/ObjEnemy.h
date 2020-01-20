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

	bool WalkTo(Vector3 target, float speed);

private:
	jAnimCSV* mAnim;
	jShaderSkin* mShader;
	jHealthPoint* mHP;

	class StateMachEnemy : public jStateMachine {
		virtual void OnPatrol();
		virtual void OnChase();
		virtual void OnAttack();
		virtual void OnDying();
	};
	StateMachEnemy* mStateMach;
};

