#pragma once

#include "junks.h"
#include "jBoneNode.h"

#include "tinyxml2.h"
using namespace tinyxml2;

class jBoneTree
{
public:
	jBoneTree();
	~jBoneTree();

	bool LoadBoneTreeDAE(string _name);
	bool LoadAnimateDAE(string _filename, string _animName);
	void Animate(float _time, vector<Matrix4>& _rets);
	void SetAnimate(string animName);

private:
	int mStartTime;
	int mEndTime;
	vector<jBoneNode> mVecBones;
	jBoneNode* mRootBone;

	void LinkTreeNodes(tinyxml2::XMLElement* _ele, jBoneNode* _parentBone);
	int SerchBoneIndex(string _name);
	tinyxml2::XMLElement* FindRootBone_DAE(tinyxml2::XMLDocument& _doc);
	float FindFrameRate(tinyxml2::XMLDocument& _doc);
	float FindAnimateEndTime(tinyxml2::XMLDocument& _doc);
};

