#pragma once
#include "jGameObject.h"
#include "jMatrixControl.h"
#include "Vectors.h"

class jShaderSkinned;
class jModel;
class jTexture;
class jAnimate;

class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

private:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnDraw();

	jModel* mModel;
	jTexture * mTexture;
	jShaderSkinned * mShader;
	jAnimate* mAnim;
	float mAnimTime;
	jMatrixControl mTransport;
	Vector3 mDestPoint;
};

