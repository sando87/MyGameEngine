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
#include "jTerrainCollider.h"
#include "jStateMachine.h"
#include "ObjItem.h"

class StateMachEnemy : public jStateMachine 
{
private:
	double mAccTime;
	Vector2 mDestPos;
	jAnimator* mAnim;
	jGameObject* mPlayer;
	ObjTerrainMgr* mTerrain;
	virtual void OnLoad();
	virtual void OnIdle();
	virtual void OnMove();
	virtual void OnAttack();
	bool DetectPlayerAround(double round);
	bool CloseEnoughToDest();
};

class NoCrash : public jCrash 
{
	virtual void OnCollision(CrashInfo info);
};

ObjEnemy::ObjEnemy()
{
}


ObjEnemy::~ObjEnemy()
{
}

void ObjEnemy::OnLoad()
{
	DBEnemies dbEnemy;
	dbEnemy.Load(1);
	GetTransform().moveTo(Vector3(2347 + 40, 4866 - 20, 0));

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + dbEnemy.resMesh));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + dbEnemy.resImg));
	AddComponent(new jAnimator(PATH_RESOURCES + string("anim/") + dbEnemy.resAnim));
	jShaderSkin* shader = new jShaderSkin();
	shader->SetRenderOrder(RenderOrder_Skin);
	AddComponent(shader);

	mHP = new jHealthPoint(dbEnemy.spec);
	mHP->EventDeath = [&](jHealthPoint* from) {
		Death();
	};
	AddComponent(mHP);

	mStateMach = new StateMachEnemy();
	AddComponent(mStateMach);

	NoCrash * crash = new NoCrash();
	crash->SetShape(5, 10);
	AddComponent(crash);

	AddComponent(new jTerrainCollider(crash->GetRound()));

}

void ObjEnemy::OnStart()
{
	jGameObject* objPlayer = GetEngine().FindGameObject("ObjPlayer");
	jAnimator* anim = FindComponent<jAnimator>();
	anim->AddEvent("attack", 0.6f, [this, objPlayer]() {
		jHealthPoint* hp = objPlayer->FindComponent<jHealthPoint>();
		if (hp != nullptr)
			mHP->Attack(hp);
	});
	
	anim->AddEvent("attack", 1.0f, [anim]() { anim->SetAnimation("idle"); });
	anim->AddEvent("death", 0.9f, [this, objPlayer]() {
		Destroy();
	});
}

void ObjEnemy::OnUpdate()
{
}

void ObjEnemy::Death()
{
	FindComponent<jAnimator>()->SetAnimation("death");
	FindComponent<StateMachEnemy>()->SetState(StateType::DYING);
	FindComponent<NoCrash>()->SetEnable(false);
	FindComponent<jHealthPoint>()->SetEnable(false);

	ObjItem* obj = new ObjItem();
	obj->GetTransform().moveTo(GetTransform().getPos());
	GetEngine().AddGameObject(obj);
	
	GetEngine().StartCoRoutine("createNextMonster", 10000, [](CorMember& mem, bool meg) {
		jGameObjectMgr::GetInst().AddGameObject(new ObjEnemy());
		return CorCmd::Stop;
	});
}


void StateMachEnemy::OnLoad()
{
	mPlayer = GetEngine().FindGameObject("ObjPlayer");
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	mAnim = GetGameObject()->FindComponent<jAnimator>();
	SetState(StateType::IDLE);
	mAccTime = 0;
}

void StateMachEnemy::OnIdle()
{
	mAccTime += jTime::Delta();
	if (DetectPlayerAround(15))
	{
		mAccTime = 0;
		mDestPos = mPlayer->GetTransform().getPos();
		SetState(StateType::MOVE);
		mAnim->SetAnimation("walk");
		return;
	}

	//5초마다 랜덤위치로 이동
	if (mAccTime > 5)
	{
		mAccTime = 0;
		Vector2 basePos = GetGameObject()->GetTransform().getPos();
		int offX = jUtils::Random() % 20 - 10;
		int offY = jUtils::Random() % 20 - 10;
		Vector2 randomPos = basePos + Vector2(offX, offY);
		float height = 0;
		if (mTerrain->GetHeight(randomPos.x, randomPos.y, height))
		{
			mDestPos = randomPos;
			SetState(StateType::MOVE);
			mAnim->SetAnimation("walk");
		}
	}
}

void StateMachEnemy::OnMove()
{
	mAccTime += jTime::Delta();
	if (DetectPlayerAround(7))
	{
		mAccTime = 0;
		mDestPos = GetGameObject()->GetTransform().getPos();
		SetState(StateType::ATTACK);
		mAnim->SetAnimation("attack");
	}
	else if (CloseEnoughToDest())
	{
		mAccTime = 0;
		mDestPos = GetGameObject()->GetTransform().getPos();
		SetState(StateType::IDLE);
		mAnim->SetAnimation("idle");
	}
	else
		GetGameObject()->GetTransform().moveSmoothlyToward2D(mDestPos, 10, jTime::Delta());
}

void StateMachEnemy::OnAttack()
{
	mAccTime += jTime::Delta();
	//3초동안의 attack delay 개념
	if (mAccTime > 3)
	{
		mAccTime = 0;
		SetState(StateType::IDLE);
		mAnim->SetAnimation("idle");
	}
}

bool StateMachEnemy::DetectPlayerAround(double round)
{
	Vector3 pos = GetGameObject()->GetTransform().getPos();
	Vector3 playerPos = mPlayer->GetTransform().getPos();
	return playerPos.distance(pos) < round;
}

bool StateMachEnemy::CloseEnoughToDest()
{
	Vector2 myPos = GetGameObject()->GetTransform().getPos();
	return myPos.distance(mDestPos) < 1;
}

void NoCrash::OnCollision(CrashInfo info)
{
	Vector2 dir = GetCenter() - info.target->GetCenter();
	dir.normalize();
	double rate = info.dist / (GetRound() + info.target->GetRound());
	double scale = 0.3 * (rate - 1) * (rate - 1) + 0.01;
	Vector3 newPos = GetGameObject()->GetTransform().getPos() + scale * dir;
	GetGameObject()->GetTransform().moveTo(newPos);
}
