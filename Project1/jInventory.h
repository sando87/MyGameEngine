#pragma once
#include "jComponent.h"

class ObjItem;

class jInventory :
	public jComponent
{
public:
	void initItemList(vector<ObjItem*>& items);
	bool PickItem(ObjItem* item);
	bool DropItem(ObjItem* item);

protected:
	virtual void OnStart();

	void Equip(ObjItem* item);
	void UnEquip(ObjItem* item);
	list<ObjItem*> mItems;
};

