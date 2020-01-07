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
	Vector3 center = GetGameObject()->GetTransport().getPos();
	Vector3 targetCenter = target->GetGameObject()->GetTransport().getPos() + target->GetGameObject()->GetTransport().getPos();
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

jRect3D jCrash::GetRect()
{
	Vector3 center = GetGameObject()->GetTransport().getPos();
	Vector3 pos = Vector3(center.x - mShape.round, center.y - mShape.round, center.z);
	Vector3 size = Vector3(mShape.round * 2, mShape.round * 2, mShape.round * 2 + mShape.height);
	return jRect3D(pos, size);
}
