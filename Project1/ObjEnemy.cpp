#include "ObjEnemy.h"
#include "ObjCamera.h"
#include "jAnimator.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "ObjTerrainMgr.h"
#include "jInput.h"
#include "jTime.h"
#include "jCrash.h"
#include "jHealthPoint.h"

ObjEnemy::ObjEnemy()
{
}


ObjEnemy::~ObjEnemy()
{
}

void ObjEnemy::OnStart()
{
	LoadTxt("MyObject_232.txt");
	mAnim = FindComponent<jAnimator>();
	mAnim->AddEvent("attack", 0.6f, []() {_trace(); });
	mAnim->AddEvent("attack", 1.0f, [this]() { mAnim->SetAnimation("idle"); });
	mShader = FindComponent<jShaderSkin>();
	mPlayer = jGameObjectMgr::GetInst().FindGameObject("ObjPlayer");

	mHP = new jHealthPoint();
	AddComponent(mHP);

	mStateMach = new StateMachEnemy();
	AddComponent(mStateMach);

	ShaderParamsSkin& param = mShader->GetParams();
	param.material.diffuse = Vector4f(1, 1, 1, 1);
	param.light.direction = Vector4f(-1, -1, -1, 0);
	AddComponent((new jCrash())->Init(1, 2, [](jCrashs objs) {}) );

	mStateMach->InitState();
}

void ObjEnemy::OnUpdate()
{
	jGameObject::OnUpdate();

	StandOnTerrain();

	mStateMach->OnUpdate();
}

bool ObjEnemy::DetectPlayerAround(double round)
{
	Vector3 pos = GetTransport().getPos();
	Vector3 playerPos = mPlayer->GetTransport().getPos();
	return playerPos.distance(pos) < round;
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

	Vector2 myPos = mObject->GetTransport().getPos();
	double dist = myPos.distance(mPatrolPos);
	if (dist > 1)
	{
		mObject->GetTransport().moveSmoothlyToward2D(mPatrolPos, 5, jTime::Delta());
	}
	else 
	{
		if (mObject->mAnim->GetAnimation() == "walk")
		{
			mObject->mAnim->SetAnimation("idle");
			mPatrolPos = mObject->GetTransport().getPos();
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
		mPatrolPos = mObject->GetTransport().getPos();
		mAccTime = 0;
	}
	else
	{
		Vector2 pos = mObject->mPlayer->GetTransport().getPos();
		mObject->GetTransport().moveSmoothlyToward2D(pos, 5, jTime::Delta());
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

void ObjEnemy::StateMachEnemy::InitState()
{
	mObject->mAnim->SetAnimation("idle");
	mPatrolPos = mObject->GetTransport().getPos();
	mState = StateType::PATROL;
	mAccTime = 0;
}
