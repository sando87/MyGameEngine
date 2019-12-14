#pragma once
#include "junks.h"

class jBoneTree;

struct KeyFrames
{
	string name;
	float frameRate;
	float endTime;
	vector<Matrix4> keyMats;
	Matrix4& GetMat(float _time)
	{
		float a = _time / endTime;
		float b = a - (int)a;
		float c = b * endTime;
		int n = (int)(c / frameRate);
		return keyMats[n];
	}
};

class jBoneNode
{
	friend class jBoneTree;

public:
	jBoneNode() : mParentBone(nullptr), mCurrentAnim(nullptr) {}
	~jBoneNode() {}

	void SetAnimate(string name)
	{
		if (mAnimates.find(name) == mAnimates.end())
			_error();
		
		mCurrentAnim = &mAnimates[name];
	}
	void AnimateOfAllChild(float _time)
	{
		if (mCurrentAnim->keyMats.size() > 0)
			mMatAnim = mCurrentAnim->GetMat(_time);
		else
			mMatAnim = mMatLocal;

		if(mParentBone != nullptr)
			mMatAnim *= mParentBone->mMatAnim;

		int cnt = mChildBones.size();
		for (int i = 0; i < cnt; ++i)
		{
			mChildBones[i]->AnimateOfAllChild(_time);
		}
	}

public:
	int mIndex;
	string mName;
	jBoneNode* mParentBone;
	vector<jBoneNode*> mChildBones;
	unordered_map<string, KeyFrames> mAnimates;
	KeyFrames* mCurrentAnim;
	Matrix4 mMatLocal;
	Matrix4 mMatWorld;
	Matrix4 mMatWorldInv;
	Matrix4 mMatAnim;
};
