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
	case StateType::IDLE: OnIdle(); break;
	case StateType::MOVE: OnMove(); break;
	case StateType::ATTACK: OnAttack(); break;
	case StateType::DAMAGED: OnDamaged(); break;
	case StateType::STERNED: OnSterned(); break;
	case StateType::DYING: OnDying(); break;
	case StateType::DEATH: OnDeath(); break;
	default: _warn(); break;
	}
}
