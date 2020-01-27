#pragma once
#include "jGameObject.h"

class jAnimator;
class jHealthPoint;
class ObjTerrainMgr;
class StateMachEnemy;

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
	StateMachEnemy* mStateMach;
};

