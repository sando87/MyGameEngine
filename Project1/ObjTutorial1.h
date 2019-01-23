#pragma once
#include "jGameObject.h"
#include "Matrices.h"
#include "jInput.h"
class jShaderSkinned;
class jShaderSprite;
class jShaderColor;
class jModel;
class jTexture;
class jAnimate;

class ObjTutorial1 : public jGameObject
{
public:
	ObjTutorial1();
	~ObjTutorial1();

private:
	void OnStart();
	void OnUpdate();
	void OnDraw();

	jModel* mModel;
	jTexture * mTexture;
	jShaderSkinned * mShader;
	jAnimate* mAnim;
	Matrix4 mWorldMat;
	int mIdxUV;
	float mAnimTime;

	jShaderColor * mShaderColor;
	jModel* mModelX;
};

