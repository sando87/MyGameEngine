#include "jTerrainCollider.h"
#include "ObjTerrainMgr.h"
#include "jGameObjectMgr.h"
#include "jTransform.h"


jTerrainCollider::jTerrainCollider(double round) : mRound(round)
{
}


jTerrainCollider::~jTerrainCollider()
{
}

void jTerrainCollider::OnLoad()
{
	mTerrain = GetGameObject()->GetEngine().FindGameObject<ObjTerrainMgr>();
	for (int i = 0; i < 8; ++i)
	{
		double radian = DegToRad(45 * i);
		mDetectPoints[i] = Vector2(cos(radian), sin(radian)) * mRound;
	}
}

void jTerrainCollider::OnUpdate()
{
	if (mTerrain == nullptr)
		return;

	Vector3 newPos = GetGameObject()->GetTransform().getPos();
	Vector2 pos = newPos;

	bool valid = false;
	float height = 0;
	Vector2 avg = Vector2(0,0);
	int numSum = 0;
	for (int i = 0; i < 8; ++i)
	{
		Vector2 pt = pos + mDetectPoints[i];
		valid = mTerrain->GetHeight(pt.x, pt.y, height);
		if (!valid)
		{
			avg += pt;
			numSum++;
		}
	}
	if (numSum > 0)
	{
		avg /= numSum;
		double rate = pos.distance(avg) / mRound;
		double scale = 0.3 * (rate - 1) * (rate - 1) + 0.1;
		Vector2 dir = (pos - avg).normalize();
		pos += dir * scale;
		newPos.x = pos.x;
		newPos.y = pos.y;
	}

	if (mTerrain->GetHeight(pos.x, pos.y, height))
		newPos.z = height;

	GetGameObject()->GetTransform().moveTo(newPos);
}
