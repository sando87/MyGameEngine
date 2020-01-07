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

	jShaderSkin* mShader;
	jAnimCSV* mAnim;

	float mAnimTime = 0;
};

