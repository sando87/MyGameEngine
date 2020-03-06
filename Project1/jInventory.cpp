#include "jInventory.h"
#include "ObjItem.h"
#include "jHealthPoint.h"

void jInventory::initItemList(vector<ObjItem*>& items)
{
	for (ObjItem* item : items)
		mItems.push_back(item);
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
	for (ObjItem* item : mItems)
	{
		if (item->GetProperty().state == ItemState::Equipped)
			Equip(item);
	}
}

void jInventory::Equip(ObjItem * item)
{
	ItemProperty prop = item->GetProperty();
	if (prop.meshFilename.length() >= 0)
	{
		//update subobject mesh
		//update subobject texture
		//update subobject anim
		//add item to gameobject list
	}

	jGameObject* obj = GetGameObject();
	jHealthPoint* stats = obj->FindComponent<jHealthPoint>();
	if (stats != nullptr)
		stats->Effect(prop);
}

void jInventory::UnEquip(ObjItem * item)
{
}
