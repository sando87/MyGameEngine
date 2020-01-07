#pragma once
#include "jGameObject.h"

class jAnimCSV;
class jShaderSkin;

class ObjEnemy :
	public jGameObject
{
public:
	ObjEnemy();
	virtual ~ObjEnemy();

	virtual void OnStart();
	virtual void OnUpdate();

	bool WalkTo(Vector3 target, float speed);

private:
	jAnimCSV* mAnim;
	jShaderSkin* mShader;
};

