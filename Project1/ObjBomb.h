#pragma once
#include "jGameObject.h"
#include "jParabolic2.h"

class ObjEffect;

class ObjBomb :
	public jGameObject
{
public:
	ObjBomb();
	~ObjBomb();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	ObjEffect* mEffectFire;
	ObjEffect* mEffectSmoke;

	jParabolic2 mHeights;
	Vector2 mDirection;
	double mDistance;
	double mAccTime;

	Property_Setter(Vector3, Dest, Vector3())
	Property_Setter(double, Second, 0)
};

