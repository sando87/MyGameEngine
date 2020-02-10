#pragma once

#include "junks.h"
#include "jComponent.h"

class AnimationClip;
class jShaderSkin;

class jAnimator : public jComponent
{
public:
	jAnimator(string name = "");
	~jAnimator();

	virtual void OnLoad();
	virtual void OnLoad2();
	virtual void OnUpdate();

	//jAnimator��ü ���� �� OnLoad�� ����Ǳ����� ���õǴ� �Լ����̹Ƿ� �� �κ��� �����ؼ� ������ �ʿ���.
	string GetAnimation();
	void SetAnimation(string name);
	void AddEvent(string animName, float rate, function<void(void)> event);
	void AddAnimationClip(string name);
	void SplitCSVtoClips();

protected:
	float mCurrentTime;
	jShaderSkin* mShader;
	AnimationClip* mCurrentAnim;
	unordered_map<string, AnimationClip> mAnims;

	mat4s Animate(float _deltaTime);
};

class AnimationClip
{
public:
	string name;
	float frameRate;
	float endTime;
	float prevPosRate;
	vector<mat4s> keyMats;
	vector<pair<float, function<void(void)>>> events;
	mat4s Animate(float _time);
	void AddEvent(float rate, function<void(void)> event);
	void ProcEvent(float currentRate);
	void SaveToFile(string fullname);
	bool LoadFromFile(string fullname);
};