#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jShape.h"

class oCollisionMgr;
class cCollider;
class cColliderSphere;
class cColliderCylinder;
class cColliderGridPole;

class cCollider :
	public jComponent
{
public:
	virtual ~cCollider();
	virtual jShape * GetShape() = 0;

	function<void(cCollider*, CrashResult)> EventCollision;

	void InvokeCollision(cCollider* target, CrashResult result);

protected:
	oCollisionMgr* mCollisionMgr;

	Property_GetSetter(bool, EnableCollision, true)
};

class cColliderSphere :
	public cCollider
{
public:
	virtual jShape * GetShape();

protected:
	virtual void OnLoad();

	jShapeSphere mShape;

	Property_GetSetter(double, Round, 1)
	Property_GetSetter(Vector3, LocalPos, Vector3())
};

class cColliderCylinder :
	public cCollider
{
public:
	virtual jShape * GetShape();

protected:
	virtual void OnLoad();

	jShapeCylinder mShape;

	Property_GetSetter(double, Round, 1)
	Property_GetSetter(double, Height, 1)
	Property_GetSetter(Vector3, LocalPos, Vector3())
	
};

class cColliderGridPole :
	public cCollider
{
public:
	virtual jShape * GetShape();
	bool Hovered = false;

protected:
	virtual void OnLoad();

	jShapeGridCylinder mShape;

};
