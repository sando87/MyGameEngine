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

	const DBItem& GetDBItem() const { return mDBItem; }
	const DBItemResource& GetDBItemResource() const { return mDBItemResorce; }

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	void NewRandomItem();
	void StartDropAnimation();
	
	u32 mDBID;
	DBItem mDBItem;
	DBItemResource mDBItemResorce;
	DBSpecification mDBItemSpec;
	jParabolic2 mHeights;
	bool mAnimateDrop = true;
};

