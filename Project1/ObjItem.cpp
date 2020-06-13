#include "ObjItem.h"
#include "jTrigger.h"
#include "ObjPlayer.h"
#include "jInventory.h"
#include "jParserMeta.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderDefault.h"
#include "jTime.h"
#include "jTransform.h"
#include "cCollider.h"
#include "cActionReceiver.h"
#include "ObjUI.h"
#include "jTerrainCollider.h"

ObjItem::ObjItem(u32 dbID) : mDBID(dbID), jGameObject("item")
{
}

ObjItem::~ObjItem()
{
}

void ObjItem::OnLoad()
{
	if (mDBID == 0)
		NewRandomItem();
	else
	{
		mDBItem.Load(mDBID);
		mDBItemResorce.Load(mDBItem.rsrcID);
		mDBItemSpec.Load(mDBItem.spec);
	}

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + mDBItemResorce.mesh));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + mDBItemResorce.img));
	AddComponent(new jShaderDefault());

	cColliderSphere * collider = new cColliderSphere();
	collider->SetRound(2);
	AddComponent(collider);

	mHeights.Init(Vector2(0.15, 10), Vector2());

	jTerrainCollider* terrainCol = new jTerrainCollider();
	terrainCol->SetOnTerrain(false);
	terrainCol->EventUnderTerrain = [&](double height) {
		mAnimateDrop = false;
		Vector3 pos = GetTransform().getPos();
		pos.z = height;
		GetTransform().lookat(pos, pos + Vector3(0, 1, 0), Vector3(0, 0, 1));
	};
	AddComponent(terrainCol);

	cActionReceiver* action = new cActionReceiver();
	action->RegisterAction([&](jGameObject* from) {
		ObjUI* formInven = GetEngine().FindGameObject<ObjUI>();
		if (formInven->IsFull())
			StartDropAnimation();
		else
		{
			formInven->PickItem(GetDBItem());
			Destroy();
		}
	});
	action->SetAttackable(false);
	AddComponent(action);
}

void ObjItem::OnUpdate()
{
	if (mAnimateDrop)
	{
		double deltaPos = mHeights.CalcVelAcc(jTime::Delta()) * jTime::Delta();
		Vector3 pos = GetTransform().getPos();
		pos.z += deltaPos;
		GetTransform().rotateAxis(Vector3(0, 1, 0), 20);
		GetTransform().moveTo(pos);
	}
}

void ObjItem::NewRandomItem()
{
	DBSpecification newSpec;
	newSpec.pa = jUtils::Random() % 5;
	newSpec.hp = jUtils::Random() % 10;
	newSpec.Save();

	mDBItem.state = ItemState::Dropped;
	mDBItem.posIndex = 0;
	mDBItem.grade = jUtils::Random() % 5;
	mDBItem.rsrcID = (jUtils::Random() % 11) + 1;
	mDBItem.spec = newSpec.GetID();

	mDBItemSpec.hp = jUtils::Random() % 30;
	mDBItemSpec.mp = jUtils::Random() % 30;
	mDBItemSpec.count = jUtils::Random() % 3;
	
	
	mDBItemResorce.Load(mDBItem.rsrcID);
}

void ObjItem::StartDropAnimation()
{
	double height = FindComponent<jTerrainCollider>()->GetHeight();
	Vector3 pos = GetTransform().getPos();
	pos.z = height;
	GetTransform().moveTo(pos);
	mAnimateDrop = true;
	mHeights.AccX = 0;
}

