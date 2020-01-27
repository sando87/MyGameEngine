#pragma once

#include "jComponent.h"

enum StateType
{
	NONE, IDLE, MOVE, ATTACK, DAMAGED, STERNED, DYING, DEATH
};

class jStateMachine : public jComponent
{
public:
	jStateMachine();
	virtual ~jStateMachine();

	virtual void OnIdle() {}
	virtual void OnMove() {}
	virtual void OnAttack() {}
	virtual void OnDamaged() {}
	virtual void OnSterned() {}
	virtual void OnDying() {}
	virtual void OnDeath() {}

	virtual void OnUpdate();

	Property_GetSetter(StateType, State, StateType::IDLE)
};

