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

	jShaderSkin* mShader;
	jBoneTree* mBones;

	float mAnimTime = 0;

	void InitAnim();
	void SetIdle() { StartCoRoutine(mIdle, "play"); }
	void SetWalk() { StartCoRoutine(mWalk, "play"); }
	void SetRun() { StartCoRoutine(mRun, "play"); }
	void SetDeath() { StartCoRoutine(mDeath, "play"); }
	function<bool(void)> mIdle;
	function<bool(void)> mWalk;
	function<bool(void)> mRun;
	function<bool(void)> mDeath;
};

