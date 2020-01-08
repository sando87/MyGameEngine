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
	Vector3 targetCenter = target->GetGameObject()->GetTransport().getPos();
	double dist = center.distance(targetCenter);
	if (dist < mShape.round + target->GetShape().round)
		return true;
	return false;
}

void jCrash::CallbackCrash()
{
	jCrashs crashesEnter;
	jCrashs crashesKeep;
	jCrashs crashesLeave;
	for (auto iter = mCrashes.begin(); iter != mCrashes.end(); )
	{
		if (iter->second.isNew)
		{
			iter->second.isNew = false;
			iter->second.isKeep = false;
			crashesEnter->push_back(iter->second.crash);
			++iter;
		}
		else if (iter->second.isKeep)
		{
			iter->second.isKeep = false;
			crashesKeep->push_back(iter->second.crash);
			++iter;
		}
		else
		{
			crashesLeave->push_back(iter->second.crash);
			mCrashes.erase(iter++);
		}
	}
	if (mCallbacksEnter) mCallbacksEnter(crashesEnter);
	if (mCallbacksKeep) mCallbacksKeep(crashesKeep);
	if (mCallbacksLeave) mCallbacksLeave(crashesLeave);
}

jRect3D jCrash::GetRect()
{
	Vector3 center = GetGameObject()->GetTransport().getPos();
	Vector3 pos = Vector3(center.x - mShape.round, center.y - mShape.round, center.z);
	Vector3 size = Vector3(mShape.round * 2, mShape.round * 2, mShape.round * 2 + mShape.height);
	return jRect3D(pos, size);
}

void jCrash::AddCrashed(jCrash * target)
{
	if (mCrashes.find(target) != mCrashes.end())
	{
		mCrashes[target].isNew = false;
		mCrashes[target].isKeep = true;
		mCrashes[target].crash = target;
	}
	else
	{
		CrashTrigInfo info;
		info.isNew = true;
		info.isKeep = false;
		info.crash = target;
		mCrashes[target] = info;
	}
}
