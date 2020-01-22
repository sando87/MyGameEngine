#include "jStateMachine.h"



jStateMachine::jStateMachine()
{
}


jStateMachine::~jStateMachine()
{
}

void jStateMachine::OnUpdate()
{
	switch (mState)
	{
	case StateType::PATROL: OnPatrol(); break;
	case StateType::CHASE: OnChase(); break;
	case StateType::ATTACK: OnAttack(); break;
	case StateType::DAMAGED: OnDamaged(); break;
	case StateType::STERNED: OnSterned(); break;
	case StateType::DYING: OnDying(); break;
	case StateType::DEATH: OnDeath(); break;
	default: _warn(); break;
	}
}
