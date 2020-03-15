#pragma once
#include "jGameObject.h"
#include "jTinyDB.h"
#include "jParabolic2.h"

class ObjItem :
	public jGameObject
{
public:
	ObjItem();
	~ObjItem();

	void LoadDB(u32 id);
	const DBItem& GetDBItem() const { return mDBItem; }
	const DBItemResource& GetDBItemResource() const { return mDBItemResorce; }

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	void NewRandomItem();

	jParabolic2 mHeights;
	DBItem mDBItem;
	DBItemResource mDBItemResorce;
};

