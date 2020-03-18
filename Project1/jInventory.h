#pragma once
#include "jComponent.h"
#include "jTinyDB.h"

class ObjItem;
class ObjUI;

class jInventory :
	public jComponent
{
public:
	jInventory(u32 dbid) : mObjectDBID(dbid) {}

	bool PickItem(ObjItem* item);
	bool DropItem(ObjItem* item);
	list<ObjItem*>& GetItems() { return mItems; }

protected:
	virtual void OnLoad();
	virtual void OnStart();

	ObjUI* mFormInventory;

	void Equip(ObjItem* item);
	void UnEquip(ObjItem* item);
	list<ObjItem*> mItems;
	u32 mObjectDBID;
	DBClasses mDBClasses;

	jGameObject* mObjBody;
	jGameObject* mObjLeg;
	jGameObject* mObjArm;
	jGameObject* mObjFoot;
};

