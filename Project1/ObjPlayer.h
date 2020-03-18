#pragma once
#include "jGameObject.h"
#include "junks.h"
#include "jAnimator.h"

class jNavigator;
class jAnimatorGroup;
class StateMachPlayer;

class ObjPlayer : public jGameObject
{
public:
	friend class jEventPlayer;
	friend class StateMachPlayer;
	ObjPlayer();
	virtual ~ObjPlayer();

	u32 GetID() { return mDBid; }

private:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	jGameObject* CreateChild(string name, string meshFullname, string imgFullname, string animFullname);

	u32 mDBid;
	Vector2 mDestPos;
	jGameObject* mTarget = nullptr;

	jAnimatorGroup * mAnim;
	jNavigator * mNavi;
	StateMachPlayer * mState;
	list<Vector2> mWayPoints;
};

