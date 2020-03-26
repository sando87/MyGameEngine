#pragma once
#include "jGameObject.h"

class jUISystem;
class jViewImage;
class jHealthPoint;

class oFormMain :
	public jGameObject
{
public:
	oFormMain();
	virtual ~oFormMain();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	u32 mPlayerDBId;
	jHealthPoint* mPlayerHP;
	jUISystem* mForm;
	jViewImage* mHPView;
	jViewImage* mMPView;
	jViewImage* mExpView;
};

