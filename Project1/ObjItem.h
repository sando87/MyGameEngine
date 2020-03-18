#pragma once
#include "jGameObject.h"
#include "junks.h"
#include "jTinyDB.h"
#include "jParabolic2.h"

class ObjItem :
	public jGameObject
{
public:
	ObjItem(u32 dbID = 0);
	~ObjItem();

	void ResetAnimate() { mHeights.AccX = 0; }
	const DBItem& GetDBItem() const { return mDBItem; }
	const DBItemResource& GetDBItemResource() const { return mDBItemResorce; }

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	void NewRandomItem();

	u32 mDBID;
	float mHeightTerrain;
	jParabolic2 mHeights;
	DBItem mDBItem;
	DBItemResource mDBItemResorce;
};

