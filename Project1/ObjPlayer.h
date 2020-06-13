#pragma once
#include "jGameObject.h"
#include "junks.h"
#include "cUserInputDriven.h"

class jHealthPoint;
class cMovement;
class cColliderCylinder;

class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

	u32 GetID() { return mDBid; }

private:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	jGameObject* CreateChild(string name, string meshFullname, string imgFullname, string animFullname);

	u32 mDBid;
	jHealthPoint* mHP;
	cMovement * mMovement;
	cColliderCylinder* mCollider;

	void OnMouseDown(InputEventArgs args);
	void OnKeyDown(InputEventArgs args);
	void CreateBombSkill(Vector3 destPos);
	void CreateIceBolt(Vector3 destPos);

	string GetCurrentAnimName();
	void SetAnimation(string name);
	void SetAnimEvent(string name, float rate, function<void(void)> handler);

	void DoAction(jGameObject* target);
};

