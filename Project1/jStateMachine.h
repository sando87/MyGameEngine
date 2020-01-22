#pragma once

#include "jComponent.h"

enum StateType
{
	NONE, PATROL, CHASE, ATTACK, DAMAGED, STERNED, DYING, DEATH
};

class jStateMachine : public jComponent
{
public:
	jStateMachine();
	virtual ~jStateMachine();

	virtual void OnPatrol() {}
	virtual void OnChase() {}
	virtual void OnAttack() {}
	virtual void OnDamaged() {}
	virtual void OnSterned() {}
	virtual void OnDying() {}
	virtual void OnDeath() {}

	virtual void OnUpdate();

	Property_GetSetter(StateType, State, StateType::PATROL)
};

