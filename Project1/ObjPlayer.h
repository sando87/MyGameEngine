#pragma once
#include "jGameObject.h"
#include "junks.h"

class jShaderSkin;
class jAnimCSV;


class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

private:
	virtual void OnStart();
	virtual void OnUpdate();

	void WalkTo(Vector2 pos, jGameObject * obj);

	jShaderSkin* mShader;
	jAnimCSV* mAnim;

	float mAnimTime = 0;
};

