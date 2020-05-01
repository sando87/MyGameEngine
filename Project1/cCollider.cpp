#include "cCollider.h"
#include "jGameObjectMgr.h"
#include "oCollisionMgr.h"
#include "jTransform.h"
#include "jMesh.h"

cCollider::~cCollider()
{
	mCollisionMgr->SubDynamicCollider(this);
}

void cCollider::InvokeCollision(cCollider * target, CrashResult result)
{
	if (EventCollision)
		EventCollision(target, result);
}



jShape * cColliderSphere::GetShape()
{
	mShape.Round = mRound;
	mShape.Position = GetGameObject()->GetTransform().getPos() + mLocalPos;
	return &mShape;
}

void cColliderSphere::OnLoad()
{
	mCollisionMgr = GetEngine().FindGameObject<oCollisionMgr>();
	_exceptif(mCollisionMgr == nullptr, return);
	
	mShape.Parent = this;

	mCollisionMgr->AddDynamicCollider(this);
}

jShape * cColliderCylinder::GetShape()
{
	mShape.Round = mRound;
	mShape.PositionBottom = GetGameObject()->GetTransform().getPos() + mLocalPos;
	mShape.PositionTop = mShape.PositionBottom;
	mShape.PositionTop.z += mHeight;
	return &mShape;
}

void cColliderCylinder::OnLoad()
{
	mCollisionMgr = GetEngine().FindGameObject<oCollisionMgr>();
	_exceptif(mCollisionMgr == nullptr, return);

	mShape.Parent = this;

	mCollisionMgr->AddDynamicCollider(this);
}


jShape * cColliderGridPole::GetShape()
{
	return &mShape;
}

void cColliderGridPole::OnLoad()
{
	mCollisionMgr = GetEngine().FindGameObject<oCollisionMgr>();
	jMesh* mesh = GetGameObject()->FindComponent<jMesh>();
	_exceptif(mCollisionMgr == nullptr || mesh == nullptr, return);

	double step = mCollisionMgr->GetStep();
	mShape.Shapes.SetStep(step);

	mesh->Load();
	Vector3 pos = GetGameObject()->GetTransform().getPos();
	mShape.Box = mesh->GetBox() + pos;
	vector<VertexFormat>& verts = mesh->GetVerticies();
	for (VertexFormat& vert : verts)
	{
		Vector3 worldPos = pos + vert.position;
		u64 key = mShape.Shapes.ToKey(worldPos);
		if (!mShape.Shapes.IsContains(key))
		{
			Vector2 pos = mShape.Shapes.ToPosition(key);
			jShapeCylinder cylinder;
			cylinder.Parent = this;
			cylinder.PositionBottom = Vector3(pos.x, pos.y, mShape.Box.Min().z);
			cylinder.PositionTop = Vector3(pos.x, pos.y, worldPos.z);
			cylinder.Round = step * 0.5;
			mShape.Shapes[key] = cylinder;
		}
		else
		{
			jShapeCylinder& cylinder = mShape.Shapes[key];
			cylinder.PositionTop.z = max(worldPos.z, cylinder.PositionTop.z);
		}
	}

	for (auto ele : mShape.Shapes.GetGrids())
	{
		u64 key = ele.first;
		Vector2 pos = mShape.Shapes.ToPosition(key);
		mCollisionMgr->AddStaticCollider(pos, this);
	}
	
}

