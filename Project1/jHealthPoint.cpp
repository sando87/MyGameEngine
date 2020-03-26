#include "jHealthPoint.h"
#include "ObjItem.h"
#include "jGameObjectMgr.h"
#include "ObjHealthBars.h"

jHealthPoint::jHealthPoint(u32 specID)
{
	mSpecDBID = specID;
}


jHealthPoint::~jHealthPoint()
{
}

void jHealthPoint::AddEffect(const DBSpecification& spec)
{
	DBSpecification rate = CurSpec / MaxSpec;
	MaxSpec += spec;
	CurSpec = MaxSpec * rate;
}

void jHealthPoint::SubEffect(const DBSpecification& spec)
{
	DBSpecification rate = CurSpec / MaxSpec;
	MaxSpec -= spec;
	CurSpec = MaxSpec * rate;
}

void jHealthPoint::Attack(jHealthPoint* target, jHealthPoint* with)
{
	if (!target->GetEnable())
		return;

	double alpha = with != nullptr ? with->CurSpec.pa : 0;
	target->CurSpec.hp -= (CurSpec.pa + alpha);
	mObjHPBar->ShowHPBar(target->GetGameObject());
	if (target->CurSpec.hp <= 0)
	{
		target->CurSpec.hp = 0;
		if (target->EventDeath != nullptr)
			target->EventDeath(this);
		if (EventKill != nullptr)
			EventKill(target);
	}
}

void jHealthPoint::OnLoad()
{
	_warnif(mSpecDBID == 0);
	MaxSpec.Load(mSpecDBID);
	CurSpec = MaxSpec;
	mObjHPBar = GetEngine().FindGameObject<ObjHealthBars>();
}
