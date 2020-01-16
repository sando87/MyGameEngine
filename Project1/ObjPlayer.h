#pragma once
#include "jGameObject.h"
#include "junks.h"

class jShaderSkin;
class jAnimCSV;
class jAStar;

class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

private:
	virtual void OnStart();
	virtual void OnUpdate();

	void MoveTo(Vector2 pos);
	void WalkTo(Vector2 pos, jGameObject * obj);
	void StartNavigate(Vector2 pos);

	jShaderSkin* mShader;
	jAnimCSV* mAnim;
	jAStar* mAstar;

	float mAnimTime = 0;
	float mStep;
};

