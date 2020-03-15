#pragma once
#include "jComponent.h"

struct DBItem;

struct STAT
{
	double life;
	double mana;
	double shield;
	double exp;
	u32 level;

	double attackPower;
	double magicPower;
	double attackDefense;
	double magicDefense;

	double spdAttack;
	double spdMove;
	double spdLifeRecovery;
	double spdManaRecovery;
	double spdShieldRecovery;
	double spdSternRecovery;

	double ShieldResetTime;

	double criticalChance;
	double criticalPowerRate;
	double coolTimeReduction;

	double itemChance;
	STAT() {
		memset(this, 0x00, sizeof(STAT));
	}
};

class jHealthPoint :
	public jComponent
{
public:
	jHealthPoint();
	virtual ~jHealthPoint();

	STAT BasicStatus;
	STAT CurrentStatus;

	void Effect(const DBItem& item);

	virtual void OnLoad();
	virtual void OnUpdate();
	virtual void OnAttack(jGameObject* target);
	virtual void OnDamaged(const STAT& attacker);

};

