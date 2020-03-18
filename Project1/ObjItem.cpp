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
#include "ObjTerrainMgr.h"
#include "jCrash.h"

ObjItem::ObjItem(u32 dbID) : mDBID(dbID), jGameObject("item")
{
}

ObjItem::~ObjItem()
{
}

void ObjItem::OnLoad()
{
	if (mDBItem.GetID() == 0)
		NewRandomItem();
	else
	{
		mDBItem.Load(mDBID);
		mDBItemResorce.Load(mDBItem.rsrcID);
	}

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + mDBItemResorce.mesh));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + mDBItemResorce.img));
	AddComponent(new jShaderDefault());

	jCrash * crash = new jCrash();
	crash->SetShape(4, 2);
	AddComponent(crash);

	mHeights.Init(Vector2(0.15, 10), Vector2());
}

void ObjItem::OnStart()
{
	ObjTerrainMgr* terrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	Vector3 pos = GetTransform().getPos();
	terrain->GetHeight(pos.x, pos.y, mHeightTerrain);
	mHeightTerrain += 4;
	pos.z = mHeightTerrain;
	GetTransform().moveTo(pos);
}

void ObjItem::OnUpdate()
{
	if (mHeights.AccX < 0.3)
	{
		double deltaPos = mHeights.CalcVelAcc(jTime::Delta()) * jTime::Delta();
		Vector3 pos = GetTransform().getPos();
		pos.z += deltaPos;
		_echoF(deltaPos);
		GetTransform().rotateAxis(Vector3(0, 1, 0), 5);
		GetTransform().moveTo(pos);
		if (mHeights.AccX >= 0.3)
			GetTransform().lookat(pos, pos + Vector3(0, 1, 0), Vector3(0, 0, 1));
	}
}

void ObjItem::NewRandomItem()
{
	mDBItem.state = ItemState::Dropped;
	mDBItem.posIndex = 0;
	mDBItem.grade = jUtils::Random() % 5;
	mDBItem.hp = jUtils::Random() % 30;
	mDBItem.mp = jUtils::Random() % 30;
	mDBItem.cntSkill = jUtils::Random() % 3;
	mDBItem.rsrcID = (jUtils::Random() % 11) + 1;

	mDBItemResorce.Load(mDBItem.rsrcID);
}

