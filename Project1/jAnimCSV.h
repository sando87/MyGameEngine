#pragma once

#include "junks.h"
#include "jComponent.h"

struct AnimCSVInfo
{
	string name;
	float frameRate;
	float endTime;
	float prevPosRate;
	vector<mat4s> keyMats;
	vector<pair<float, function<void(void)>>> events;
	mat4s Animate(float _time)
	{
		float a = _time / endTime;
		float b = a - (int)a;
		float c = b * endTime;
		int n = (int)(c / frameRate);
		ProcEvent(b);
		return keyMats[n];
	}
	void AddEvent(float rate, function<void(void)> event)
	{
		events.push_back(make_pair(rate, event));
	}
	void ProcEvent(float currentRate)
	{
		float fixedRate = currentRate < prevPosRate ? currentRate + 1.0f : currentRate;
		for (auto item : events)
		{
			float eventRate = item.first;
			if (prevPosRate < eventRate && eventRate < fixedRate)
				item.second();
		}
		prevPosRate = currentRate;
	}
};

class jAnimCSV : public jComponent
{
public:
	jAnimCSV(string name = "");
	~jAnimCSV();

	bool Load(string _filename);
	mat4s Animate(float _deltaTime);
	void SetAnimation(string name);
	void AddEvent(string name, float rate, function<void(void)> event);
	string GetCurrentAnim() { return mCurrentAnim->name; }
	AnimCSVInfo* GetAnimationInfo(string name) { 
		if (mAnims.find(name) == mAnims.end())
			return nullptr;
		return &mAnims[name];
	}

private:
	float mCurrentTime;
	AnimCSVInfo* mCurrentAnim;
	unordered_map<string, AnimCSVInfo> mAnims;
};

