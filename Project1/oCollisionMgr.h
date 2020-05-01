#pragma once
#include "jGameObject.h"
#include "junks.h"
#include "jRectangle2D.h"
#include "cCollider.h"

struct CollisionInfo
{
	cCollider* left;
	cCollider* right;
	u64 key;
};
struct CollisionGrid
{
	u64 key;
	unordered_map<cCollider*, cCollider*> colliders;
};

class oCollisionMgr :
	public jGameObject
{
	friend class jShaderColliderBoxes;
public:
	void AddStaticCollider(jRectangle2D worldRect, cCollider* collider);
	void AddStaticCollider(Vector2 pos, cCollider* collider);
	void AddDynamicCollider(cCollider* collider);
	void SubDynamicCollider(cCollider* collider);

	cCollider* CrashedCollider(Vector3 pos);
	void GetColliders(jRectangle2D rect, vector<cCollider*>& colliders);
	CrashResult RayCast(Vector3 startPos, Vector3 endPos);
	
protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	unordered_map<u64, CollisionGrid> mGridsStatic;
	unordered_map<u64, CollisionGrid> mGridsDynamic;
	list<cCollider*> mDynamicColliders;

	u64 ToIndex(double worldPos) { return (u64)((worldPos + mStep / 2) / mStep); }
	u64 ToKey(Vector2 worldPos) { return (u64)((ToIndex(worldPos.y) << 32) | ToIndex(worldPos.x)); }
	void GetKeys(jRectangle2D rect, vector<u64>& keys);
	void GetKeys(Vector3 startPos, Vector3 endPos, vector<u64>& keys);
	void GetColliders(u64 key, vector<cCollider*>& colliders);
	void GetColliders(vector<u64>& keys, vector<cCollider*>& colliders);
	void InvokeColliderEvent(unordered_map<u64, CollisionInfo>& pairs);
	void CandidatePairs(unordered_map<u64, CollisionInfo>& pairs);
	void UpdateDynamicGrid();

	Property_Getter(double, Step, 3.0)
};

