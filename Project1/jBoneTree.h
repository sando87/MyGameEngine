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

	bool Load(string _name);
	bool LoadBoneTreeDAE(string _name);
	bool LoadAnimateDAE(string _name);
	void Animate(float _time, vector<Matrix4>& _rets);

private:
	int mStartTime;
	int mEndTime;
	vector<jBoneNode> mVecBones;
	jBoneNode* mRootBone;

	void ProcNode(tinyxml2::XMLElement* _ele, jBoneNode* _parentBone);
	int SerchBoneIndex(string _name);
	tinyxml2::XMLElement* FindRootBone_DAE(tinyxml2::XMLDocument& _doc);
};

