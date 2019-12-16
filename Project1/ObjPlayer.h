#pragma once
#include "jGameObject.h"
#include "junks.h"

class jShaderSkin;
class jBoneTree;


class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

private:
	virtual void OnStart();
	virtual void OnUpdate();

	jShaderSkin* mShader;
	jBoneTree* mBones;

	float mAnimTime = 0;
	void SetAnim(string animName);
};

