#pragma once

#include "junks.h"
#include "jComponent.h"

class AnimationClip;

class jAnimator : public jComponent
{
public:
	jAnimator(string name = "");
	~jAnimator();

	virtual void OnLoad();

	mat4s Animate(float _deltaTime);
	void AddEvent(string name, float rate, function<void(void)> event);
	void SetAnimation(string name);
	string GetAnimation();

private:
	float mCurrentTime;
	AnimationClip* mCurrentAnim;
	unordered_map<string, AnimationClip> mAnims;
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
};