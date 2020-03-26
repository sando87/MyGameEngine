#pragma once
#include "jComponent.h"
#include "jTinyDB.h"

class ObjHealthBars;

class jHealthPoint :
	public jComponent
{
public:
	jHealthPoint(u32 specID);
	virtual ~jHealthPoint();

	DBSpecification MaxSpec;
	DBSpecification CurSpec;

	function<void(jHealthPoint*)> EventDeath;
	function<void(jHealthPoint*)> EventKill;

	void AddEffect(const DBSpecification& spec);
	void SubEffect(const DBSpecification& spec);
	void Attack(jHealthPoint* target, jHealthPoint* with = nullptr);

protected:
	virtual void OnLoad();

	ObjHealthBars* mObjHPBar;

	u32 mSpecDBID;
};

