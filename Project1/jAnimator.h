#pragma once

#include "junks.h"
#include "jComponent.h"

class AnimationClip;
class jShaderSkin;
class jHealthPoint;

class jAnimator : public jComponent
{
public:
	jAnimator(string name = "");
	~jAnimator();

	virtual void OnLoad();
	virtual void OnUpdate();

	//jAnimator객체 생성 후 OnLoad가 수행되기전에 세팅되는 함수들이므로 그 부분을 감안해서 구현이 필요함.
	string GetAnimation();
	void SetAnimation(string name);
	void SetEvent(string animName, float rate, function<void(void)> event);
	void ClearEvent(string animName);

protected:
	float mPreTimeRate;
	jShaderSkin* mShader;
	jHealthPoint* mHP;
	AnimationClip* mCurrentAnim;
	unordered_map<string, AnimationClip> mAnims;

	mat4s Animate(float _deltaTime);
	float GetAnimationSpeed();
	void SplitCSVtoClips();
};

class AnimationClip
{
public:
	int magic;
	int headerLen;
	string name;
	string animFilename;
	string meshInfo;
	string distPerLoop;
	bool isLoop;
	float moveSpeed;
	float endTime;
	int boneCount;
	int keyCount;
	vector<mat4s> keyMats;
	unordered_map<float, function<void(void)>> events;

	string HeaderToString() {
		string str = name + "," 
			+ animFilename + "," 
			+ meshInfo + "," 
			+ (isLoop?"loop":"noloop") + "," 
			+ distPerLoop + ","
			+ jUtils::ToString(endTime) + ","
			+ jUtils::ToString(boneCount) + "," 
			+ jUtils::ToString(keyCount);
		return str;
	}
	void StringToHead(string str) {
		strings pieces = jUtils::Split2(str, ",");
		_exceptif(!pieces, return);
		name = pieces[0];
		animFilename = pieces[1];
		meshInfo = pieces[2];
		isLoop = pieces[3] == "loop" ? true : false;
		distPerLoop = pieces[4];
		endTime = jUtils::ToDouble(pieces[5]);
		moveSpeed = jUtils::ToDouble(distPerLoop) / endTime;
		boneCount = jUtils::ToInt(pieces[6]);
		keyCount = jUtils::ToInt(pieces[7]);
	}
	void SaveToFile(string fullname);
	bool LoadFromFile(string fullname);
};

class AnimationClipOri
{
public:
	string name;
	float endTime;
	float moveSpeed;
	vector<mat4s> keyMats;
	unordered_map<float, function<void(void)>> events;
	void SaveToFile(string fullname);
	bool LoadFromFile(string fullname);
};