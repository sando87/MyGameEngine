#include "oCollisionMgr.h"
#include "cCollider.h"

void oCollisionMgr::AddPointStatic(cCollider* collider)
{
	CollisionGrid grid;
	Vector3 pos = collider->GetWorldPos();
	grid.key = ToKey(pos);
	grid.height = pos.z;
	grid.colliders.push_back(collider);
	mGridsStatic[grid.key] = grid;
}

void oCollisionMgr::AddPointDynamic(cCollider* collider)
{
	CollisionGrid grid;
	Vector3 pos = collider->GetWorldPos();
	grid.key = ToKey(pos);
	grid.height = pos.z;
	grid.colliders.push_back(collider);
	mGridsDynamic[grid.key] = grid;
}

void oCollisionMgr::OnUpdate()
{
	InspectCollision();
}

void oCollisionMgr::InspectCollision()
{
}

void oCollisionMgr::CandidatePairs(unordered_map<void*, pair<cCollider*, cCollider*>>& pairs)
{
}
