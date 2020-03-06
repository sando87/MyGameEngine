#include "jHealthPoint.h"
#include "ObjItem.h"


jHealthPoint::jHealthPoint()
{
}


jHealthPoint::~jHealthPoint()
{
}

void jHealthPoint::Effect(ItemProperty & item)
{
}

void jHealthPoint::OnLoad()
{
}

void jHealthPoint::OnUpdate()
{
}

void jHealthPoint::OnAttack(jGameObject* target)
{
	jHealthPoint* targetStat = target->FindComponent<jHealthPoint>();
	if (targetStat == nullptr)
		return;

	targetStat->OnDamaged(CurrentStatus);
}

void jHealthPoint::OnDamaged(const STAT & attacker)
{
	CurrentStatus.life -= attacker.attackPower;
}
