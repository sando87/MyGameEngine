#include "ObjEnemy.h"
#include "ObjCamera.h"
#include "jAnimCSV.h"
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
	mAnim = FindComponent<jAnimCSV>();
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
	mat4s mats = mAnim->Animate(jTime::Delta());
	ShaderParamsSkin& param = mShader->GetParams();
	for (int i = 0; i < 45; ++i)
		param.bones[i] = mats[i];

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
	ObjEnemy* me = (ObjEnemy*)mGameObject;
	mAccTime += jTime::Delta();
	if (me->DetectPlayerAround(15))
	{
		mAccTime = 0;
		SetState(StateType::CHASE);
		me->mAnim->SetAnimation("walk");
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
		me->mAnim->SetAnimation("walk");
	}

	Vector2 myPos = me->GetTransport().getPos();
	double dist = myPos.distance(mPatrolPos);
	if (dist > 1)
	{
		me->GetTransport().moveSmoothlyToward2D(mPatrolPos, 10, jTime::Delta());
	}
	else 
	{
		if (me->mAnim->GetCurrentAnim() == "walk")
		{
			me->mAnim->SetAnimation("idle");
			mPatrolPos = me->GetTransport().getPos();
		}
	}
}

void ObjEnemy::StateMachEnemy::OnChase()
{
	ObjEnemy* me = (ObjEnemy*)mGameObject;
	if (me->DetectPlayerAround(5))
	{
		SetState(StateType::ATTACK);
		me->mAnim->SetAnimation("attack");
	}
	else if (!me->DetectPlayerAround(20))
	{
		SetState(StateType::PATROL);
		me->mAnim->SetAnimation("idle");
		mPatrolPos = me->GetTransport().getPos();
		mAccTime = 0;
	}
	else
	{
		Vector2 pos = me->mPlayer->GetTransport().getPos();
		me->GetTransport().moveSmoothlyToward2D(pos, 10, jTime::Delta());
	}
}

void ObjEnemy::StateMachEnemy::OnAttack()
{
	ObjEnemy* me = (ObjEnemy*)mGameObject;
	mAccTime += jTime::Delta();
	//3초마다 다음 State선정
	if (mAccTime > 3)
	{
		mAccTime = 0;
		if (me->DetectPlayerAround(5))
		{
			SetState(StateType::ATTACK);
			me->mAnim->SetAnimation("attack");
		}
		else
		{
			SetState(StateType::CHASE);
			me->mAnim->SetAnimation("walk");
		}
	}
}

void ObjEnemy::StateMachEnemy::InitState()
{
	ObjEnemy* me = (ObjEnemy*)mGameObject;
	me->mAnim->SetAnimation("idle");
	mPatrolPos = me->GetTransport().getPos();
	mState = StateType::PATROL;
	mAccTime = 0;
}
