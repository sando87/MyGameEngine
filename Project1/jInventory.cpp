#include "jInventory.h"
#include "ObjItem.h"
#include "jHealthPoint.h"
#include "jGameObjectMgr.h"
#include "ObjUI.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "jTinyDB.h"

bool jInventory::PickItem(ObjItem * item)
{
	return true;
}

bool jInventory::DropItem(ObjItem * item)
{
	DBItem DBItem = item->GetDBItem();
	DBItem.owner = 0;
	DBItem.Save();

	return true;
}

void jInventory::OnLoad()
{
	mObjBody = GetGameObject()->FindGameObject("body");
	mObjLeg = GetGameObject()->FindGameObject("leg");
	mObjArm = GetGameObject()->FindGameObject("arm");
	mObjFoot = GetGameObject()->FindGameObject("foot");

	DBPlayer dbPlayer;
	dbPlayer.Load(mObjectDBID);
	mDBClasses.Load(dbPlayer.classes);

	vector<DBItem> items;
	jTinyDB::GetInst().ReadRecords(items, [this](TinyRecord* rec) {
		return rec->GetValue<u32>("owner") == mObjectDBID;
	});

	for (DBItem& item : items)
	{
		ObjItem* obj = new ObjItem(item.GetID());
		mItems.push_back(obj);
	}
}

void jInventory::OnStart()
{
}

void jInventory::Equip(ObjItem * item)
{
	const DBItemResource& resInfo = item->GetDBItemResource();
	if (resInfo.category == "armor")
	{
		jMesh* compMesh = mObjBody->FindComponent<jMesh>();
		jImage* compImg = mObjBody->FindComponent<jImage>();
		jShaderSkin* compshader = mObjBody->FindComponent<jShaderSkin>();
		compMesh->SetFullname(PATH_RESOURCES + string("mesh/") + resInfo.equipMesh);
		compImg->SetFullname(PATH_RESOURCES + string("img/") + resInfo.equipImg);
		compshader->UpdateInputResource();
	}
	else if (resInfo.category == "pants")
	{
	}
	else if (resInfo.category == "gloves")
	{
	}
	else if (resInfo.category == "shoes")
	{
	}

}

void jInventory::UnEquip(ObjItem * item)
{
	const DBItemResource& resInfo = item->GetDBItemResource();
	if (resInfo.category == "armor")
	{
		jMesh* compMesh = mObjBody->FindComponent<jMesh>();
		jImage* compImg = mObjBody->FindComponent<jImage>();
		jShaderSkin* compshader = mObjBody->FindComponent<jShaderSkin>();
		compMesh->SetFullname(PATH_RESOURCES + string("mesh/") + mDBClasses.bodyMesh);
		compImg->SetFullname(PATH_RESOURCES + string("img/") + mDBClasses.bodyImg);
		compshader->UpdateInputResource();
	}
	else if (resInfo.category == "pants")
	{
	}
	else if (resInfo.category == "gloves")
	{
	}
	else if (resInfo.category == "shoes")
	{
	}

}
