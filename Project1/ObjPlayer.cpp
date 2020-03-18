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
#include "jInputEvent.h"
#include "jTerrainCollider.h"
#include "jNavigator.h"
#include "jStateMachine.h"
#include "jInventory.h"
#include "ObjItem.h"
#include "jTinyDB.h"
#include "jTrigger.h"
#include "ObjUI.h"

class jEventPlayer : public jInputEvent
{
private:
	ObjPlayer * mPlayer;
	ObjCamera * mCamera;
	ObjTerrainMgr * mTerrain;
	virtual void OnLoad();
	virtual void OnMouseDown(Vector2n pt, int type);
};
class jAnimatorGroup : public jComponent
{
public:
	string GetAnimation();
	void SetAnimation(string name);
	void AddEvent(string name, float rate, function<void(void)> event);
protected:
	virtual void OnLoad();

	vector<jAnimator*> mAnimators;
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
	mDBid = 1;
	DBPlayer dbPlayer;
	dbPlayer.Load(mDBid);
	GetTransform().moveTo(dbPlayer.startPos);

	//AddComponent(new jInventory(mDBid));
	
	double xx = dbPlayer.startPos.x;
	double yy = dbPlayer.startPos.y;
	GetEngine().StartCoRoutine("createNewItem", 3000, [this, xx, yy](CorMember& mem, bool meg) {
		ObjItem* obj = new ObjItem();
		obj->GetTransform().moveTo(Vector3(xx + 10, yy + 10, 0));
		GetEngine().AddGameObject(obj);
		return CorCmd::Stop;
	});

	DBClasses dbClasses;
	dbClasses.Load(dbPlayer.classes);
	CreateChild("body", dbClasses.bodyMesh, dbClasses.bodyImg, dbClasses.bodyAnim);
	CreateChild("leg",  dbClasses.legMesh, dbClasses.legImg, dbClasses.legAnim);
	CreateChild("arm",  dbClasses.armMesh, dbClasses.armImg, dbClasses.armAnim);
	CreateChild("foot", dbClasses.footMesh, dbClasses.footImg, dbClasses.footAnim);

	mAnim = new jAnimatorGroup();
	AddComponent(mAnim);

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
	mAnim->AddEvent("attack1", 1.0f, [this]() { mAnim->SetAnimation("idle"); });
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
	jShaderSkin* shader = new jShaderSkin();
	shader->SetRenderOrder(RenderOrder_Skin);
	child->AddComponent(shader);
	AddChild(child);
	return child;
}


string jAnimatorGroup::GetAnimation()
{
	jAnimator* firstAnimator = mAnimators.front();
	return firstAnimator->GetAnimation();
}

void jAnimatorGroup::SetAnimation(string name)
{
	for (jAnimator* animator : mAnimators)
		animator->SetAnimation(name);
}

void jAnimatorGroup::AddEvent(string name, float rate, function<void(void)> event)
{
	jAnimator* firstAnimator = mAnimators.front();
	firstAnimator->AddEvent(name, rate, event);
}

void jAnimatorGroup::OnLoad()
{
	list<jGameObject*> childs = GetGameObject()->GetChilds();
	for (jGameObject* child : childs)
	{
		jAnimator* anim = child->FindComponent<jAnimator>();
		if(anim != nullptr)
			mAnimators.push_back(anim);
	}
}


void jEventPlayer::OnLoad()
{
	mPlayer = GetEngine().FindGameObject<ObjPlayer>();
	mCamera = GetEngine().FindGameObject<ObjCamera>();
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	if (mTerrain == nullptr)
		SetEnable(false);
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
		mPlayer->mTarget = GetEngine().RayCast(camPos, view);
	}

	
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
				if (mPlayer->mTarget->GetName() == "item")
				{
					ObjItem* objItem = (ObjItem*)mPlayer->mTarget;
					ObjUI* formInven = GetEngine().FindGameObject<ObjUI>();
					if (formInven->IsFull())
						objItem->ResetAnimate();
					else
					{
						formInven->PickItem(objItem->GetDBItem());
						objItem->Destroy();
					}

					SetState(StateType::IDLE);
					mPlayer->mAnim->SetAnimation("idle");
				}
				else
				{
					SetState(StateType::ATTACK);
					mPlayer->mAnim->SetAnimation("attack1");
				}
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
			mPlayer->mAnim->SetAnimation("attack1");
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
