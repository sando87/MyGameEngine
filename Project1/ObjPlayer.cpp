#include "ObjPlayer.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "oSkillIcebolt.h"
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
#include "jTerrainCollider.h"
#include "jNavigator.h"
#include "jInventory.h"
#include "ObjItem.h"
#include "jTinyDB.h"
#include "jTrigger.h"
#include "ObjUI.h"
#include "oFormMain.h"
#include "oFormStats.h"
#include "ObjBomb.h"
#include "jHealthPoint.h"
#include "cMovement.h"
#include "cCollider.h"
#include "cActionReceiver.h"

ObjPlayer::ObjPlayer()
{
}

ObjPlayer::~ObjPlayer()
{
}

void ObjPlayer::OnLoad()
{
	mDBid = 1;
	DBPlayer dbPlayer;
	dbPlayer.Load(mDBid);
	GetTransform().Zoom(Vector3(1.3, 1.3, 1.3));
	GetTransform().moveTo(dbPlayer.startPos);

	DBClasses dbClasses;
	dbClasses.Load(dbPlayer.classes);
	CreateChild("body", dbClasses.bodyMesh, dbClasses.bodyImg, dbClasses.bodyAnim);
	CreateChild("leg",  dbClasses.legMesh, dbClasses.legImg, dbClasses.legAnim);
	CreateChild("arm",  dbClasses.armMesh, dbClasses.armImg, dbClasses.armAnim);
	CreateChild("foot", dbClasses.footMesh, dbClasses.footImg, dbClasses.footAnim);

	AddComponent(new jHealthPoint(dbClasses.spec));

	mMovement = new cMovement();
	mMovement->EventDetected = [&](jGameObject* target) {
		if (nullptr == target)
		{
			SetAnimation("idle");
			mMovement->Stop();
		}
		else
		{
			DoAction(target);
		}
	};
	AddComponent(mMovement);

	mCollider = new cColliderCylinder();
	mCollider->SetHeight(7);
	mCollider->SetRound(1);
	mCollider->EventCollision = [&](vector<CrashResult>& retsults) {
		if (mMovement->IsMoving())
		{
			_trace();
			if (false == mMovement->Navigate())
			{
				SetAnimation("idle");
				mMovement->Stop();
			}
		}
	};
	AddComponent(mCollider);

	cUserInputDriven* playerEvent = new cUserInputDriven();
	playerEvent->EventMouseDown = [&](InputEventArgs args) { 
		OnMouseDown(args);
		return EventResult::TransferEvent;
	};
	playerEvent->EventKeyDown = [&](InputEventArgs args) { 
		OnKeyDown(args);
		return EventResult::TransferEvent;
	};
	AddComponent(playerEvent);

	//mNavi = new jNavigator();
	//AddComponent(mNavi);

	AddComponent(new jTerrainCollider());
}

void ObjPlayer::OnStart()
{
	DBPlayer dbPlayer;
	dbPlayer.Load(mDBid);
	DBSpecification spec;
	spec.pa = 0.5 * (dbPlayer.statsPA + dbPlayer.level);
	spec.ma = 0.5 * (dbPlayer.statsMA + dbPlayer.level);
	spec.pd = 0.2 * (dbPlayer.statsPD + dbPlayer.level);
	spec.md = 0.1 * (dbPlayer.statsMD + dbPlayer.level);
	spec.hp = 1.0 * (dbPlayer.statsHP + dbPlayer.level);
	spec.mp = 0.3 * (dbPlayer.statsMP + dbPlayer.level);
	mHP = FindComponent<jHealthPoint>();
	mHP->AddEffect(spec);
	mHP->EventKill = [&](jHealthPoint* target) {
		DBPlayer dbPlayer;
		dbPlayer.Load(mDBid);
		dbPlayer.exp += 10;
		if (dbPlayer.exp >= dbPlayer.level * 100)
		{
			dbPlayer.level += 1;
			dbPlayer.statsRemain += 5;
		}
		dbPlayer.Save();
	};

	SetAnimEvent("attack", 1.0f, [&]() { SetAnimation("idle"); });
	SetAnimEvent("cast", 1.0f, [&]() { SetAnimation("idle"); });
}
void ObjPlayer::OnUpdate()
{
}

jGameObject* ObjPlayer::CreateChild(string name, string meshFullname, string imgFullname, string animFullname)
{
	jGameObject* child = new jGameObject(name);
	child->AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + meshFullname));
	child->AddComponent(new jImage(PATH_RESOURCES + string("img/") + imgFullname));
	child->AddComponent(new jAnimator(PATH_RESOURCES + string("anim/") + animFullname));

	//jMesh* mesh = new jMesh();
	//mesh->LoadCube(4);
	//child->AddComponent(mesh);
	//child->AddComponent(new jImage(PATH_RESOURCES + string("img/") + imgFullname));
	//child->AddComponent(new jAnimator(PATH_RESOURCES + string("anim/") + animFullname));

	jShaderSkin* shader = new jShaderSkin();
	shader->SetRenderOrder(RenderOrder_Skin);
	child->AddComponent(shader);
	AddChild(child);
	return child;
}

void ObjPlayer::OnMouseDown(InputEventArgs args)
{
	string curState = GetCurrentAnimName();
	if ("idle" != curState && "walk" != curState)
		return;

	if (nullptr != args.hoveredObject)
	{
		cActionReceiver* action = args.hoveredObject->FindComponent<cActionReceiver>();
		if (nullptr != action)
		{
			SetAnimation("walk");
			mMovement->Move(args.hoveredObject, 5);
			return;
		}
	}

	SetAnimation("walk");
	mMovement->Move(args.terrainPt);
}

void ObjPlayer::OnKeyDown(InputEventArgs args)
{
	switch (args.key)
	{
	case 'Q':
	case 'W':
	case 'E':
	{
		string name = GetCurrentAnimName();
		if (name == "idle" || name == "walk")
		{
			Vector3 destPos = args.terrainPt;
			SetAnimation("cast");
			SetAnimEvent("cast", 0.5f, [&, destPos]() {
				CreateBombSkill(destPos);
			});
		}
		break;
	}
	case 'R':
	{
		string name = GetCurrentAnimName();
		if (name == "idle" || name == "walk")
		{
			Vector3 destPos = args.terrainPt;
			SetAnimation("cast");
			SetAnimEvent("cast", 0.5f, [&, destPos]() {
				CreateIceBolt(destPos);
			});
		}
		break;
	}
	default:
		break;
	}
}

void ObjPlayer::CreateBombSkill(Vector3 destPos)
{
	Vector3 playerPos = GetTransform().getPos();
	ObjBomb* objBomb = new ObjBomb();
	objBomb->GetTransform().moveTo(playerPos);
	objBomb->SetDestPos(destPos);
	objBomb->SetOwner(this);
	GetEngine().AddGameObject(objBomb);
}
void ObjPlayer::CreateIceBolt(Vector3 destPos)
{
	Vector3 playerPos = GetTransform().getPos();
	oSkillIcebolt* icdBolt = new oSkillIcebolt();
	icdBolt->SetStartPos(playerPos);
	icdBolt->SetDestPos(destPos);
	GetEngine().AddGameObject(icdBolt);
}

string ObjPlayer::GetCurrentAnimName()
{
	jAnimator* anim = mChilds.front()->FindComponent<jAnimator>();
	_exceptif(nullptr == anim, return "");
	return anim->GetAnimation();
}

void ObjPlayer::SetAnimation(string name)
{
	for (jGameObject* child : mChilds)
	{
		jAnimator* anim = child->FindComponent<jAnimator>();
		_exceptif(nullptr == anim, break);
		anim->SetAnimation(name);
	}
}

void ObjPlayer::SetAnimEvent(string name, float rate, function<void(void)> handler)
{
	jAnimator* anim = mChilds.front()->FindComponent<jAnimator>();
	_exceptif(nullptr == anim, return);
	anim->SetEvent(name, rate, handler);
}

void ObjPlayer::DoAction(jGameObject* target)
{
	cActionReceiver* action = target->FindComponent<cActionReceiver>();
	if (nullptr == action)
		return;

	if (action->GetAttackable())
	{
		mMovement->Stop();
		SetAnimation("attack");
		SetAnimEvent("attack", 0.6f, [this, action]() {
			action->InvokeAction(this);
		});
	}
	else
	{
		mMovement->Stop();
		SetAnimation("idle");
		action->InvokeAction(this);
	}
}

