#include "jInventory.h"
#include "ObjItem.h"
#include "jHealthPoint.h"
#include "jGameObjectMgr.h"
#include "ObjUI.h"

void jInventory::LoadItems(vector<u32>& items)
{
	for (u32 id : items)
	{
		ObjItem* obj = new ObjItem();
		obj->LoadDB(id);
		mItems.push_back(obj);
	}
}

bool jInventory::PickItem(ObjItem * item)
{
	return true;
}

bool jInventory::DropItem(ObjItem * item)
{
	return true;
}

void jInventory::OnStart()
{
	mFormInventory = GetEngine().FindGameObject<ObjUI>();

	for (ObjItem* item : mItems)
	{
		mFormInventory->AddItem(item);
		if (item->GetDBItem().state == ItemState::Equipped)
			Equip(item);
	}
}

void jInventory::Equip(ObjItem * item)
{
	const DBItemResource& resInfo = item->GetDBItemResource();
	if (resInfo.equipMesh.length() >= 0)
	{
		//update subobject mesh
		//update subobject texture
		//update subobject anim
		//add item to gameobject list
	}

	jGameObject* obj = GetGameObject();
	jHealthPoint* stats = obj->FindComponent<jHealthPoint>();
	if (stats != nullptr)
		stats->Effect(item->GetDBItem());
}

void jInventory::UnEquip(ObjItem * item)
{
}
