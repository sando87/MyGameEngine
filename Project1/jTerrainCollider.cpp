#include "jTerrainCollider.h"
#include "ObjTerrainMgr.h"
#include "jGameObjectMgr.h"
#include "jTransform.h"


jTerrainCollider::jTerrainCollider()
{
}


jTerrainCollider::~jTerrainCollider()
{
}

double jTerrainCollider::GetHeight()
{
	float height = 0;
	bool ret = mTerrain->GetHeight(GetGameObject()->GetTransform().getPos(), height);
	_exceptif(false == ret, return 0);
	return height;
}

void jTerrainCollider::OnLoad()
{
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	_exceptif(mTerrain == nullptr, return);
}

void jTerrainCollider::OnStart()
{
	Vector3 pos = GetGameObject()->GetTransform().getPos();
	float height = 0;
	mTerrain->GetHeight(pos, height);
	GetGameObject()->GetTransform().setHeight(height);
}

void jTerrainCollider::OnUpdate()
{
	if (mTerrain == nullptr)
		return;

	float height = 0;
	Vector3 pos = GetGameObject()->GetTransform().getPos();
	bool ret = mTerrain->GetHeight(pos, height);
	_exceptif(false == ret, return);

	if (mOnTerrain)
		GetGameObject()->GetTransform().setHeight(height);

	if (pos.z < height - 1e-4)
	{
		if (EventUnderTerrain)
			EventUnderTerrain(height);
	}
}
