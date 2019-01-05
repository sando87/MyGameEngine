#pragma once
#include <string>
#include <vector>
#include "Matrices.h"
using namespace std;

class jAnimate;

struct KeyFrames
{
	string name;
	vector<float> times;
	vector<Matrix4> keyMats;
	Matrix4& GetMat(float _time)
	{
		float step = times[1] - times[0];
		int n = (int)(_time / step);
		int mod = n % times.size();
		return keyMats[mod];
	}
};

class jBoneNode
{
	friend class jAnimate;

public:
	jBoneNode();
	~jBoneNode();

	void AnimateOfAllChild(float _time)
	{
		if (mKeyFrames.times.size() > 0)
			mMatAnim = mKeyFrames.GetMat(_time);
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
	KeyFrames mKeyFrames;
	Matrix4 mMatLocal;
	Matrix4 mMatWorld;
	Matrix4 mMatWorldInv;
	Matrix4 mMatAnim;
};
