#pragma once

#include "junks.h"
#include "jComponent.h"

struct AnimCSVInfo
{
	string name;
	float frameRate;
	float endTime;
	vector<mat4s> keyMats;
	mat4s GetMat(float _time)
	{
		float a = _time / endTime;
		float b = a - (int)a;
		float c = b * endTime;
		int n = (int)(c / frameRate);
		return keyMats[n];
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
	void SetAnimation(string ToName, string BackName);
	string GetCurrentAnim() { return mCurrentAnim->name; }
	AnimCSVInfo* GetAnimationInfo(string name) { 
		if (mAnims.find(name) == mAnims.end())
			return nullptr;
		return &mAnims[name];
	}

private:
	float mCurrentTime;
	AnimCSVInfo* mCurrentAnim;
	AnimCSVInfo* mReturnAnim;
	unordered_map<string, AnimCSVInfo> mAnims;
};

