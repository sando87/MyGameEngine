#include "jCrash.h"
#include "jMatrixControl.h"


jCrash::jCrash()
{
}


jCrash::~jCrash()
{
}

bool jCrash::Crashed(jCrash * target)
{
	Vector3 center = mShape.center + GetGameObject()->GetTransport().getPos();
	Vector3 targetCenter = target->GetShape().center + target->GetGameObject()->GetTransport().getPos();
	double dist = center.distance(targetCenter);
	if (dist < mShape.round + target->GetShape().round)
		return true;
	return false;
}

void jCrash::OnCrash()
{
	if(mCrashs)
		mCallback(mCrashs);

	mCrashs->clear();
}

jRect jCrash::GetRect()
{
	Vector3 center = mShape.center + GetGameObject()->GetTransport().getPos();
	return jRect(center.x - mShape.round, center.y - mShape.round, center.x + mShape.round, center.y + mShape.round);
}
