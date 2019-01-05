#pragma once

#include <string>
#include <vector>
#include "Matrices.h"
#include "jBoneNode.h"
#include "tinyxml2.h"
using namespace std;
using namespace tinyxml2;

class jAnimate
{
public:
	jAnimate();
	~jAnimate();

	bool Load(string _name);
	bool LoadBoneTreeDAE(string _name);
	bool LoadAnimateDAE(string _name);
	void Animate(float _time, vector<Matrix4>& _rets);

public:
	int mStartTime;
	int mEndTime;
	vector<jBoneNode> mVecBones;
	jBoneNode* mRootBone;

	void ProcNode(XMLElement* _ele, jBoneNode* _parentBone);
	int SerchBoneIndex(string _name);
	XMLElement* FindRootBone_DAE(tinyxml2::XMLDocument& _doc);
};

