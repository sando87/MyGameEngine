#include "ObjPlayer.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jMesh.h"
#include "jShaderSkin.h"
#include "jTransform.h"
#include "jImage.h"
#include "jTime.h"
#include "jBoneTree.h"
#include "jOS_APIs.h"
#include "jCrash.h"
#include "jAnimator.h"
#include "jLine3D.h"
#include "jParserMeta.h"
#include "jInputEvent.h"
#include "jTerrainCollider.h"
#include "jNavigator.h"
#include "jStateMachine.h"

class jEventPlayer : public jInputEvent
{
private:
	ObjPlayer * mPlayer;
	ObjCamera * mCamera;
	ObjTerrainMgr * mTerrain;
	virtual void OnLoad();
	virtual void OnMouseDown(Vector2n pt, int type);
};
class jAnimatorGroup
{
public:
	void AddChild(jAnimator* anim);
	string GetAnimation();
	void SetAnimation(string name);
	void AddEvent(string name, float rate, function<void(void)> event);
protected:
	vector<jAnimator*> mChildAnimators;
};
class MyCrash : public jCrash 
{
	virtual void OnCollision(CrashInfo info) 
	{
		Vector2 dir = GetCenter() - info.target->GetCenter();
		dir.normalize();
		double rate = info.dist / (GetRound() + info.target->GetRound());
		double scale = 0.3 * (rate - 1) * (rate - 1) + 0.01;
		Vector3 newPos = GetGameObject()->GetTransform().getPos() + scale * dir;
		GetGameObject()->GetTransform().moveTo(newPos);
	}
};
class StateMachPlayer : public jStateMachine
{
private:
	virtual void OnLoad();
	virtual void OnIdle();
	virtual void OnMove();
	virtual void OnAttack();

	ObjPlayer * mPlayer;
	bool CloseEnoughToTarget();
	bool CloseEnoughToDest(Vector2 dest);
};

ObjPlayer::ObjPlayer()
{
}


ObjPlayer::~ObjPlayer()
{
}

void ObjPlayer::OnLoad()
{
	jParserMeta meta;
	meta.Load(PATH_RESOURCES + string("meta/") + "MyObject_397_P.txt");

	mAnim = new jAnimatorGroup();
	vector<string> childs = meta.GetValues(MF_Child);
	for (string fullnameChild : childs)
	{
		jGameObject* child = new jGameObject();
		child->LoadTxt(fullnameChild);
		AddChild(child);
		GetEngine().AddGameObject(child);
		mAnim->AddChild(child->FindComponent<jAnimator>());
	}

	mAnim->AddEvent("attack", 1.0f, [this]() { mAnim->SetAnimation("idle"); });
	mAnim->SetAnimation("idle");

	GetTransform().moveTo(meta.GetValue<Vector3>(MF_WorldPos));

	AddComponent(new jEventPlayer());

	mState = new StateMachPlayer();
	AddComponent(mState);

	mNavi = new jNavigator();
	AddComponent(mNavi);

	MyCrash * crash = new MyCrash();
	crash->SetShape(2, 5);
	AddComponent(crash);

	AddComponent(new jTerrainCollider(crash->GetRound()));
}

void ObjPlayer::OnStart()
{
}
void ObjPlayer::OnUpdate()
{
}


void jAnimatorGroup::AddChild(jAnimator * anim)
{
	mChildAnimators.push_back(anim);
}

string jAnimatorGroup::GetAnimation()
{
	jAnimator* firstAnimator = mChildAnimators.front();
	return firstAnimator->GetAnimation();
}

void jAnimatorGroup::SetAnimation(string name)
{
	for (jAnimator* animator : mChildAnimators)
		animator->SetAnimation(name);
}

void jAnimatorGroup::AddEvent(string name, float rate, function<void(void)> event)
{
	jAnimator* firstAnimator = mChildAnimators.front();
	firstAnimator->AddEvent(name, rate, event);
}

void jEventPlayer::OnLoad()
{
	mPlayer = GetEngine().FindGameObject<ObjPlayer>();
	mCamera = GetEngine().FindGameObject<ObjCamera>();
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
}

void jEventPlayer::OnMouseDown(Vector2n pt, int type)
{
	Vector3 view = mCamera->ScreenToWorldView(pt.x, pt.y);
	Vector3 camPos = mCamera->GetTransform().getPos();
	Vector3 playerPos = mPlayer->GetTransform().getPos();

	if (type == 1)
	{
		Vector2 pointOnTerrain;
		Vector2 obstaclePos;
		mTerrain->RayCastTerrain(camPos, view, pointOnTerrain);
		bool obstacled = mTerrain->FindObstacle(playerPos, pointOnTerrain, obstaclePos, 1);
		mPlayer->mDestPos = obstaclePos;
		mPlayer->mWayPoints.clear();
		if (obstacled)
		{
			mPlayer->mNavi->StartNavigate(pointOnTerrain, [this](list<Vector2>& waypoints) {
				mPlayer->mWayPoints.clear();
				if (!waypoints.empty())
				{
					mPlayer->mDestPos = waypoints.back();
					mPlayer->mWayPoints = waypoints;
					mPlayer->mState->SetState(StateType::MOVE);
					mPlayer->mAnim->SetAnimation("walk");
				}
			});
		}
		mPlayer->mState->SetState(StateType::MOVE);
		mPlayer->mAnim->SetAnimation("walk");
	}

	//player->mTarget = GetEngine().RayCast(camPos, view);
}

void StateMachPlayer::OnLoad()
{
	mPlayer = (ObjPlayer *)GetGameObject();
}

void StateMachPlayer::OnIdle()
{
}

void StateMachPlayer::OnMove()
{
	if (mPlayer->mTarget != nullptr)
	{
		if (mPlayer->mWayPoints.empty())
		{
			GetGameObject()->GetTransform().moveSmoothlyToward2D(mPlayer->mTarget->GetTransform().getPos(), 20, jTime::Delta());
			if (CloseEnoughToTarget())
			{
				SetState(StateType::ATTACK);
				mPlayer->mAnim->SetAnimation("attack");
			}
		}
		else
		{
			Vector2 dest = mPlayer->mWayPoints.front();
			GetGameObject()->GetTransform().moveSmoothlyToward2D(dest, 20, jTime::Delta());
			if (CloseEnoughToDest(dest))
			{
				mPlayer->mWayPoints.pop_front();
				if (mPlayer->mWayPoints.size() == 1)
					mPlayer->mWayPoints.clear();
			}
		}
	}
	else
	{
		if (mPlayer->mWayPoints.empty())
		{
			GetGameObject()->GetTransform().moveSmoothlyToward2D(mPlayer->mDestPos, 20, jTime::Delta());
			if (CloseEnoughToDest(mPlayer->mDestPos))
			{
				SetState(StateType::IDLE);
				mPlayer->mAnim->SetAnimation("idle");
			}
		}
		else
		{
			Vector2 dest = mPlayer->mWayPoints.front();
			GetGameObject()->GetTransform().moveSmoothlyToward2D(dest, 20, jTime::Delta());
			if (CloseEnoughToDest(dest))
			{
				mPlayer->mWayPoints.pop_front();
				if (mPlayer->mWayPoints.empty())
				{
					SetState(StateType::IDLE);
					mPlayer->mAnim->SetAnimation("idle");
				}
			}
		}
	}
}

void StateMachPlayer::OnAttack()
{
	if (mPlayer->mTarget == nullptr)
		return;

	if (CloseEnoughToTarget())
	{
		if (mPlayer->mAnim->GetAnimation() == "idle")
			mPlayer->mAnim->SetAnimation("attack");
	}
	else
	{
		SetState(StateType::MOVE);
		mPlayer->mAnim->SetAnimation("walk");
	}
}

bool StateMachPlayer::CloseEnoughToTarget()
{
	Vector2 targetPos = mPlayer->mTarget->GetTransform().getPos();
	Vector2 myPos = mPlayer->GetTransform().getPos();
	return targetPos.distance(myPos) < 3;
}

bool StateMachPlayer::CloseEnoughToDest(Vector2 dest)
{
	Vector2 myPos = mPlayer->GetTransform().getPos();
	return dest.distance(myPos) < 1;
}
