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

class jTriggeredByObject : public jTrigger
{
	virtual void OnTriggered(jGameObject* object);
};

ObjItem::ObjItem()
{
}

ObjItem::~ObjItem()
{
}

void ObjItem::LoadDB(u32 id)
{
	mDBItem.Load(id);
	mDBItemResorce.Load(mDBItem.rsrcID);
}

void ObjItem::OnLoad()
{
	if (mDBItem.GetID() == 0)
		NewRandomItem();

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + mDBItemResorce.mesh));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + mDBItemResorce.img));
	AddComponent(new jShaderDefault());

	AddComponent(new jTriggeredByObject());
	mHeights.Init(Vector2(0.15, 10), Vector2());
}

void ObjItem::OnUpdate()
{
	if (mHeights.AccX < 0.3)
	{
		double height = mHeights.CalcYAcc(jTime::Delta());
		Vector3 pos = GetTransform().getPos();
		pos.z = height;
		GetTransform().rotateAxis(Vector3(0, 1, 0), 5);
		GetTransform().moveTo(pos);
	}

	Vector3 pos = GetTransform().getPos();
	if(pos.z < 0)
	{
		pos.z = 0;
		GetTransform().lookat(Vector3(), Vector3(0, 1, 0), Vector3(0, 0, 1));
		GetTransform().moveTo(pos);
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

void jTriggeredByObject::OnTriggered(jGameObject * object)
{
	jInventory* inven = object->FindComponent<jInventory>();
	if (inven == nullptr)
		return;

	inven->PickItem((ObjItem*)GetGameObject());
}

