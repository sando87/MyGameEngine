#include "ObjEnemy.h"
#include "ObjCamera.h"
#include "jAnimator.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "ObjTerrainMgr.h"
#include "jTime.h"
#include "jHealthPoint.h"
#include "jTerrainCollider.h"
#include "ObjItem.h"
#include "cCollider.h"
#include "cUserInputDriven.h"
#include "cActionReceiver.h"
#include "cMovement.h"
#include "ObjPlayer.h"

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
	GetTransform().Zoom(Vector3(1.3, 1.3, 1.3));

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + dbEnemy.resMesh));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + dbEnemy.resImg));
	AddComponent(new jAnimator(PATH_RESOURCES + string("anim/") + dbEnemy.resAnim));
	mAnim = FindComponent<jAnimator>();

	jShaderSkin* shader = new jShaderSkin();
	shader->SetRenderOrder(RenderOrder_Skin);
	AddComponent(shader);

	mMovement = new cMovement();
	mMovement->EventDetected = [&](jGameObject* target) {
		if (nullptr == target)
		{
			mAnim->SetAnimation("idle");
			mMovement->Stop();
		}
		else
		{
			AttackObject(target);
		}
	};
	AddComponent(mMovement);

	mHP = new jHealthPoint(dbEnemy.spec);
	mHP->EventDeath = [&](jHealthPoint* from) {
		Death();
	};
	AddComponent(mHP);

	AddComponent(new jTerrainCollider());

	mCollider = new cColliderCylinder();
	mCollider->SetRound(1);
	mCollider->SetHeight(7);
	AddComponent(mCollider);

	cUserInputDriven* driven = new cUserInputDriven();
	driven->EventHoverEnter = [&]() { FindComponent<jShaderSkin>()->GetParamBasic().matDiffuse = Vector4(1.5, 1.5, 1.5, 1); };
	driven->EventHoverLeave = [&]() { FindComponent<jShaderSkin>()->GetParamBasic().matDiffuse = Vector4(1, 1, 1, 1); };
	AddComponent(driven);

	cActionReceiver* action = new cActionReceiver();
	action->RegisterAction([&](jGameObject* from) {
		jHealthPoint* attacker = from->FindComponent<jHealthPoint>();
		if (nullptr != attacker)
			attacker->Attack(mHP);
	});
	action->SetAttackable(true);
	AddComponent(action);
}

void ObjEnemy::OnStart()
{
	mAnim->SetEvent("attack1", 1.0f, [&]() { mAnim->SetAnimation("idle"); });
	mAnim->SetEvent("attack2", 1.0f, [&]() { mAnim->SetAnimation("idle"); });
	mAnim->SetEvent("dying", 0.9f, [&]() { Destroy(); });
}

void ObjEnemy::OnUpdate()
{
	mAccTime += jTime::Delta();
	if (mAccTime >= mNextActionTime)
	{
		DoAction();

		double randomDelta = ((jUtils::Random() % 40) - 20) * 0.1;
		mNextActionTime = 5.0 + randomDelta; //3.0~7.0 sec interval
		mAccTime = 0;
	}
}

void ObjEnemy::DoAction()
{
	string curAnim = mAnim->GetAnimation();
	if ("idle" != curAnim && "walk" != curAnim)
		return;

	jGameObject* target = IsAroundTarget(20);
	if (nullptr != target)
	{
		mMovement->Move(target, mCollider->GetRound() * 2);
		mAnim->SetAnimation("walk");
	}
	else
	{
		if (jUtils::Random() % 2 == 0)
		{
			Vector2 basePos = GetTransform().getPos();
			int offX = jUtils::Random() % 20 - 10;
			int offY = jUtils::Random() % 20 - 10;
			Vector2 randomPos = basePos + Vector2(offX, offY);
			mMovement->Move(randomPos);
			mAnim->SetAnimation("walk");
		}
		else
		{
			mMovement->Stop();
			mAnim->SetAnimation("idle");
		}
	}
}

void ObjEnemy::AttackObject(jGameObject * obj)
{
	if (jUtils::Random() % 2 == 0)
	{
		mMovement->Stop();
		mAnim->SetAnimation("attack1");
		mAnim->SetEvent("attack1", 0.6f, [this, obj]() {
			jHealthPoint* hp = obj->FindComponent<jHealthPoint>();
			if (hp != nullptr)
				mHP->Attack(hp);
		});
	}
	else
	{
		mMovement->Stop();
		mAnim->SetAnimation("attack2");
		mAnim->SetEvent("attack2", 0.6f, [this, obj]() {
			jHealthPoint* hp = obj->FindComponent<jHealthPoint>();
			if (hp != nullptr)
				mHP->Attack(hp);
		});
	}
}

jGameObject * ObjEnemy::IsAroundTarget(double round)
{
	ObjPlayer* player = GetEngine().FindGameObject<ObjPlayer>();
	if (nullptr == player)
		return nullptr;

	Vector3 pos = GetTransform().getPos();
	Vector3 playerPos = player->GetTransform().getPos();
	return playerPos.distance2D(pos) < round ? player : nullptr;
}
void ObjEnemy::Death()
{
	mMovement->Stop();
	FindComponent<jAnimator>()->SetAnimation("dying");
	FindComponent<jHealthPoint>()->SetEnable(false);

	ObjItem* obj = new ObjItem();
	obj->GetTransform().moveTo(GetTransform().getPos());
	GetEngine().AddGameObject(obj);

	GetEngine().StartCoRoutine("createNextMonster", 10000, [](CorMember& mem, bool meg) {
		jGameObjectMgr::GetInst().AddGameObject(new ObjEnemy());
		return CorCmd::Stop;
	});
}

