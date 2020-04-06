#pragma once
#include "jGameObject.h"
#include "junks.h"

class cCollider;

struct CollisionGrid
{
	u64 key;
	double height;
	list<cCollider*> colliders;
};

class oCollisionMgr :
	public jGameObject
{
public:
	void AddPointStatic(cCollider* collider);
	void AddPointDynamic(cCollider* collider);
protected:
	virtual void OnUpdate();

	const double mStep = 3.0;
	unordered_map<u64, CollisionGrid> mGridsStatic;
	unordered_map<u64, CollisionGrid> mGridsDynamic;

	u64 ToIndex(double worldPos) { return (u64)((worldPos + mStep / 2) / mStep); }
	u64 ToKey(Vector2 worldPos) { return (u64)((ToIndex(worldPos.y) << 32) | ToIndex(worldPos.x)); }
	void InspectCollision();
	void CandidatePairs(unordered_map<void*, pair<cCollider*, cCollider*>>& pairs);
};

