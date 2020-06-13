#include "cMovement.h"
#include "ObjCamera.h"
#include "jGameObjectMgr.h"
#include "oCollisionMgr.h"
#include "jTransform.h"
#include "jTime.h"
#include "jHealthPoint.h"
#include "cCollider.h"
#include "ObjTerrainMgr.h"

cMovement::cMovement()
{
}


cMovement::~cMovement()
{
}

void cMovement::OnLoad()
{
	mMoving = false;
	mTarget = nullptr;
	mCamera = GetEngine().FindGameObject<ObjCamera>();
	mCollision = GetEngine().FindGameObject<oCollisionMgr>();
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	_exceptif(mCamera == nullptr || mCollision == nullptr, return);

	mCollider = GetGameObject()->FindComponent<cCollider>();
	mHP = GetGameObject()->FindComponent<jHealthPoint>();
	
	mAstar.Moveable = [this](Vector2 worldPos) {
		double r = GetRound();
		jRectangle2D rect(worldPos.x - r, worldPos.y - r, 2 * r, 2 * r);
		return IsAccessable(rect);
	};

	if (EventDetected == nullptr)
		EventDetected = [](jGameObject*) {};
}

void cMovement::OnUpdate()
{
	if (!mMoving)
		return;

	Vector2 nextDest = NextDestPos();
	jTransform& trans = GetGameObject()->GetTransform();
	trans.moveSmoothlyToward2D(nextDest, GetSpeed(), jTime::Delta());

	if (GetDestination().distance(Vector2(trans.getPos())) <= mDetectRange + 1e-4)
		EventDetected(mTarget);
}

bool cMovement::IsObstacle(Vector2 destPos)
{
	if (false == mCollider)
		return true;

	jRectangle2D rect = mCollider->GetShape()->GetBox().Top();
	double round = GetRound();
	Vector2 curPos = GetGameObject()->GetTransform().getPos();
	Vector2 dir = destPos - curPos;
	double dist = dir.length();
	dir.normalize();
	for (double s = round; s < dist; s += round)
	{
		jRectangle2D nextRect = rect + (dir * s);
		if (false == IsAccessable(nextRect))
			return true;
	}
	return false;
}

bool cMovement::IsAccessable(jRectangle2D rect)
{
	if (false == mCollider)
		return true;

	jShapeSphere sphere;
	sphere.Position = rect.GetCenter();
	float height = 0;
	mTerrain->GetHeight(sphere.Position, height);
	sphere.Position.z = height;
	sphere.Round = rect.GetRound();
	vector<cCollider*> colliders;
	mCollision->GetColliders(rect, colliders);
	for (cCollider* col : colliders)
	{
		if (col->GetGameObject() == GetGameObject() || col->GetGameObject() == mTarget)
			continue;

		CrashResult ret = sphere.IsCrash(col->GetShape());
		if (ret.isCrash)
			return false;
	}
	return true;
}

Vector2 cMovement::NextDestPos()
{
	jTransform& trans = GetGameObject()->GetTransform();
	if (!mWaypoints.empty())
	{
		Vector2 nextDest = mWaypoints.front();
		if (nextDest.distance(Vector2(trans.getPos())) <= 1e-4)
		{
			mWaypoints.pop_front();
			if (!mWaypoints.empty())
				return mWaypoints.front();
		}
		else
			return nextDest;
	}

	return GetDestination();
}

Vector2 cMovement::GetDestination()
{
	return (mTarget == nullptr) ? mDestination : Vector2(mTarget->GetTransform().getPos());
}

double cMovement::GetSpeed()
{
	double velocity_ms = (mHP == nullptr) ? 1.0 : mHP->CurSpec.MoveSpeed;
	return MeterToWorldUnit(velocity_ms);
}

double cMovement::GetRound()
{
	if (nullptr == mCollider)
		return 2.0;

	jRectangle2D rect = mCollider->GetShape()->GetBox().Top();
	return (rect.Width() + rect.Height()) / 2.0;
}

void cMovement::Move(Vector2 destPos, double detectRange)
{
	mWaypoints.clear();
	mDestination = destPos;
	mDetectRange = detectRange;
	mTarget = nullptr;
	mMoving = true;

	if (IsObstacle(mDestination))
		Navigate();
}

void cMovement::Move(jGameObject * target, double detectRange)
{
	mWaypoints.clear();
	mDetectRange = detectRange;
	mTarget = target;
	mDestination = mTarget->GetTransform().getPos();
	mMoving = true;

	if (IsObstacle(mDestination))
		Navigate();
}

void cMovement::Stop()
{
	mMoving = false;
	mDestination = GetGameObject()->GetTransform().getPos();
	mTarget = nullptr;
	mWaypoints.clear();
}

bool cMovement::Navigate()
{
	double step = mCollision->GetStep();
	mWaypoints.clear();
	mAstar.StopRouting();
	mAstar.Route(GetGameObject()->GetTransform().getPos(), NextDestPos(), 300, step);
	vector<Vector2>& rets = mAstar.GetResults();
	for (Vector2 pos : rets)
		mWaypoints.push_front(pos);

	return mWaypoints.empty() ? false : true;
}
