#include "cMovement.h"
#include "ObjCamera.h"
#include "jGameObjectMgr.h"
#include "oCollisionMgr.h"
#include "jTransform.h"
#include "jTime.h"
#include "jHealthPoint.h"
#include "cCollider.h"

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
	_exceptif(mCamera == nullptr || mCollision == nullptr, return);

	mCollider = GetGameObject()->FindComponent<cCollider>();
	mHP = GetGameObject()->FindComponent<jHealthPoint>();
	
	mAstar.Moveable = [this](Vector2 worldPos) {
		double r = GetRound();
		jRectangle2D rect(worldPos.x - r, worldPos.y - r, 2 * r, 2 * r);
		vector<cCollider*> colliders;
		mCollision->GetColliders(rect, colliders);
		if (colliders.empty())
			return true;
		else if (colliders[0]->GetGameObject() == GetGameObject())
			return true;
		else if (colliders[0]->GetGameObject() == mTarget)
			return true;
		return false;
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
	return mCollider == nullptr ? 2.0 : mCollider->GetShape()->GetBox().Top().GetSize().length() * 0.5;
}

void cMovement::Move(Vector2 destPos, double detectRange)
{
	mWaypoints.clear();
	mDestination = destPos;
	mDetectRange = detectRange;
	mTarget = nullptr;
	mMoving = true;
}

void cMovement::Move(jGameObject * target, double detectRange)
{
	mWaypoints.clear();
	mDetectRange = detectRange;
	mTarget = target;
	mDestination = mTarget->GetTransform().getPos();
	mMoving = true;
}

void cMovement::Stop()
{
	mMoving = false;
	mDestination = GetGameObject()->GetTransform().getPos();
	mTarget = nullptr;
	mWaypoints.clear();
}

void cMovement::Navigate()
{
	double step = mCollision->GetStep();
	mWaypoints.clear();
	mAstar.StopRouting();
	mAstar.Route(GetGameObject()->GetTransform().getPos(), NextDestPos(), 300, step);
	vector<Vector2>& rets = mAstar.GetResults();
	for (Vector2 pos : rets)
		mWaypoints.push_front(pos);

	if (rets.empty())
		Stop();
	else
		mMoving = true;
}
