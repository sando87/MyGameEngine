#pragma once
#include "jComponent.h"

class ObjItem;
class ObjUI;

class jInventory :
	public jComponent
{
public:
	void initItemList(vector<ObjItem*>& items);
	bool PickItem(ObjItem* item);
	bool DropItem(ObjItem* item);
	list<ObjItem*>& GetItems() { return mItems; }

protected:
	virtual void OnStart();

	ObjUI* mFormInventory;

	void Equip(ObjItem* item);
	void UnEquip(ObjItem* item);
	list<ObjItem*> mItems;
};

