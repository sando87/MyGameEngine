#include "ObjEnemy.h"
#include "ObjCamera.h"
#include "jAnimator.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "ObjTerrainMgr.h"
#include "jTime.h"
#include "jCrash.h"
#include "jHealthPoint.h"

ObjEnemy::ObjEnemy()
{
}


ObjEnemy::~ObjEnemy()
{
}

void ObjEnemy::OnLoad()
{
	LoadTxt("MyObject_232.txt");

	mHP = new jHealthPoint();
	AddComponent(mHP);

	mStateMach = new StateMachEnemy();
	AddComponent(mStateMach);
}

void ObjEnemy::OnStart()
{
	mAnim = FindComponent<jAnimator>();
	mAnim->AddEvent("attack", 0.6f, []() {_trace(); });
	mAnim->AddEvent("attack", 1.0f, [this]() { mAnim->SetAnimation("idle"); });

	mPlayer = GetEngine().FindGameObject("ObjPlayer");
	mTerrain = (ObjTerrainMgr *)GetEngine().FindGameObject("ObjTerrainMgr");

	jCrash* crash = new jCrash();
	crash->SetShape(new ShapeCapsule());
	crash->SetEventEnter([](jCrashInfos objs) {});
}

void ObjEnemy::OnUpdate()
{
	Vector3 pos = GetTransform().getPos();
	float height = 0;
	bool ret = mTerrain->GetHeight(pos.x, pos.y, height);
	if (ret)
	{
		pos.z = height;
		GetTransform().moveTo(pos);
	}
}

bool ObjEnemy::DetectPlayerAround(double round)
{
	Vector3 pos = GetTransform().getPos();
	Vector3 playerPos = mPlayer->GetTransform().getPos();
	return playerPos.distance(pos) < round;
}

void ObjEnemy::StateMachEnemy::OnLoad()
{
	mObject = (ObjEnemy*)GetGameObject();
	mObject->FindComponent<jAnimator>()->SetAnimation("idle");
	mPatrolPos = mObject->GetTransform().getPos();
	SetState(StateType::PATROL);
	mAccTime = 0;
}

void ObjEnemy::StateMachEnemy::OnPatrol()
{
	mAccTime += jTime::Delta();
	if (mObject->DetectPlayerAround(15))
	{
		mAccTime = 0;
		SetState(StateType::CHASE);
		mObject->mAnim->SetAnimation("walk");
		return;
	}

	//5초마다 랜덤위치로 이동
	if (mAccTime > 5)
	{
		mAccTime = 0;
		Vector2 basePos(1390, 820);
		int offX = jUtils::Random() % 20 - 10;
		int offY = jUtils::Random() % 20 - 10;
		mPatrolPos = basePos + Vector2(offX, offY);
		mObject->mAnim->SetAnimation("walk");
	}

	Vector2 myPos = mObject->GetTransform().getPos();
	double dist = myPos.distance(mPatrolPos);
	if (dist > 1)
	{
		mObject->GetTransform().moveSmoothlyToward2D(mPatrolPos, 10, jTime::Delta());
	}
	else 
	{
		if (mObject->mAnim->GetAnimation() == "walk")
		{
			mObject->mAnim->SetAnimation("idle");
			mPatrolPos = mObject->GetTransform().getPos();
		}
	}
}

void ObjEnemy::StateMachEnemy::OnChase()
{
	if (mObject->DetectPlayerAround(5))
	{
		SetState(StateType::ATTACK);
		mObject->mAnim->SetAnimation("attack");
	}
	else if (!mObject->DetectPlayerAround(20))
	{
		SetState(StateType::PATROL);
		mObject->mAnim->SetAnimation("idle");
		mPatrolPos = mObject->GetTransform().getPos();
		mAccTime = 0;
	}
	else
	{
		Vector2 pos = mObject->mPlayer->GetTransform().getPos();
		mObject->GetTransform().moveSmoothlyToward2D(pos, 10, jTime::Delta());
	}
}

void ObjEnemy::StateMachEnemy::OnAttack()
{
	mAccTime += jTime::Delta();
	//3초마다 다음 State선정
	if (mAccTime > 3)
	{
		mAccTime = 0;
		if (mObject->DetectPlayerAround(5))
		{
			SetState(StateType::ATTACK);
			mObject->mAnim->SetAnimation("attack");
		}
		else
		{
			SetState(StateType::CHASE);
			mObject->mAnim->SetAnimation("walk");
		}
	}
}

