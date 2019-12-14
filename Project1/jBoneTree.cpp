#include "jBoneTree.h"


jBoneTree::jBoneTree()
{
}


jBoneTree::~jBoneTree()
{
}

bool jBoneTree::LoadBoneTreeDAE(string _name)
{
	if (jUtils::GetFileExtension(_name) != "DAE")
		return false;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(_name.c_str());

	if (string(doc.FirstChildElement()->Name()) != "COLLADA")
		return false;

	if (doc.FirstChildElement("COLLADA")->FirstChildElement("library_controllers") == nullptr)
		return false;

	XMLElement* rootBone = FindRootBone_DAE(doc);
	if (rootBone == nullptr)
		return false;

	string boneNames = doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_controllers")
		->FirstChildElement("controller")
		->FirstChildElement("skin")
		->FirstChildElement("source")
		->FirstChildElement("Name_array")->GetText();

	vector<string> boneIndicies;
	jUtils::Split(boneNames, " \n", boneIndicies);
	int cntBones = boneIndicies.size();
	mVecBones.clear();
	mVecBones.reserve(100);
	mVecBones.resize(cntBones);
	for (int i = 0; i < cntBones; ++i)
	{
		mVecBones[i].mIndex = i;
		mVecBones[i].mName = boneIndicies[i];
	}

	LinkTreeNodes(rootBone, nullptr);

	doc.Clear();
	return true;
}

bool jBoneTree::LoadAnimateDAE(string _filename, string _animName)
{
	if (jUtils::GetFileExtension(_filename) != "DAE")
		return false;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(_filename.c_str());

	if (string(doc.FirstChildElement()->Name()) != "COLLADA")
		return false;

	if (doc.FirstChildElement("COLLADA")->FirstChildElement("library_animations") == nullptr)
		return false;

	float frameRate = FindFrameRate(doc);
	float animateEndTime = FindAnimateEndTime(doc);
	XMLElement* eleNode = doc.FirstChildElement("COLLADA")->FirstChildElement("library_animations")->FirstChildElement("animation");
	while (true)
	{
		if (eleNode == nullptr || strcmp(eleNode->Name(), "animation"))
			break;

		const char* animName;
		eleNode->FirstChildElement("animation")->FirstChildElement("channel")->QueryStringAttribute("target", &animName);
		vector<string> vecNames;
		jUtils::Split(string(animName), "/", vecNames);
		string boneName = vecNames[0];
		int idx = SerchBoneIndex(boneName);
		if (idx < 0)
			break;

		mVecBones[idx].mAnimates[_animName] = KeyFrames();
		KeyFrames& key = mVecBones[idx].mAnimates[_animName];
		key.name = boneName;
		key.frameRate = 1 / frameRate;
		key.endTime = animateEndTime;
		key.keyMats.clear();

		vector<float> vecTimesFloat;
		vector<string> vecTimes;
		string times = eleNode->FirstChildElement("animation")->FirstChildElement("source")->FirstChildElement("float_array")->GetText();
		jUtils::Split(times, " \n", vecTimes);
		for (int i = 0; i < vecTimes.size(); ++i)
			vecTimesFloat.push_back(stof(vecTimes[i]));

		vector<string> vecMats;
		string mats = eleNode->FirstChildElement("animation")->FirstChildElement("source")->NextSiblingElement()->FirstChildElement("float_array")->GetText();
		jUtils::Split(mats, " \n", vecMats);
		int cnt = vecMats.size() / 16;
		for (int i = 0; i < cnt; i++)
		{
			if (vecTimesFloat[i] > animateEndTime)
				break;

			Matrix4 mat;
			for (int j = 0; j < 16; ++j)
			{
				mat[j] = stod(vecMats[i * 16 + j]);
			}
			mat.transpose();
			key.keyMats.push_back(mat);
		}

		eleNode = eleNode->NextSiblingElement();
	}

	doc.Clear();
	return true;
}

XMLElement* jBoneTree::FindRootBone_DAE(tinyxml2::XMLDocument& _doc)
{
	XMLElement* ele = _doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_visual_scenes")
		->FirstChildElement("visual_scene")
		->FirstChildElement("node");
	if (ele == nullptr)
		return nullptr;

	while (true)
	{
		const char *val = nullptr;
		auto err = ele->QueryStringAttribute("type", &val);
		if (err != XML_NO_ATTRIBUTE && string(val) == "JOINT")
			return ele;

		ele = ele->NextSiblingElement();
	}

	return nullptr;
}

float jBoneTree::FindFrameRate(tinyxml2::XMLDocument & _doc)
{
	XMLElement* ele = _doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_visual_scenes")
		->FirstChildElement("visual_scene")
		->FirstChildElement("extra")
		->FirstChildElement("technique");
	if (ele == nullptr)
		return 0;


	while (true)
	{
		const char *profile = nullptr;
		auto err = ele->QueryStringAttribute("profile", &profile);
		if (err != XML_NO_ATTRIBUTE && string(profile) == "MAX3D")
		{
			const char *val = ele->FirstChildElement("frame_rate")->GetText();
			return jUtils::ToDouble(val);
		}

		ele = ele->NextSiblingElement();
	}

	return 0;
}

float jBoneTree::FindAnimateEndTime(tinyxml2::XMLDocument & _doc)
{
	XMLElement* ele = _doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_visual_scenes")
		->FirstChildElement("visual_scene")
		->FirstChildElement("extra")
		->FirstChildElement("technique");
	if (ele == nullptr)
		return 0;


	while (true)
	{
		const char *profile = nullptr;
		auto err = ele->QueryStringAttribute("profile", &profile);
		if (err != XML_NO_ATTRIBUTE && string(profile) == "FCOLLADA")
		{
			const char *val = ele->FirstChildElement("end_time")->GetText();
			return jUtils::ToDouble(val);
		}

		ele = ele->NextSiblingElement();
	}

	return 0;
}

void jBoneTree::LinkTreeNodes(XMLElement * _ele, jBoneNode* _parentBone)
{
	const char* name = nullptr;
	_ele->QueryStringAttribute("id", &name);
	int boneIdx = SerchBoneIndex(name);
	if (boneIdx < 0)
	{
		jBoneNode node;
		node.mIndex = mVecBones.size();
		node.mName = name;
		mVecBones.push_back(node);
		boneIdx = node.mIndex;
	}

	jBoneNode& bone = mVecBones[boneIdx];
	bone.mParentBone = _parentBone;
	const char* mats = _ele->FirstChildElement("matrix")->GetText();
	string mat(mats);
	vector<string> matrix;
	jUtils::Split(mat, " \n", matrix);
	Vector4 row[4];
	for (int i = 0; i < 4; ++i)
	{
		row[i] = Vector4(stof(matrix[i * 4 + 0]), stof(matrix[i * 4 + 1]), stof(matrix[i * 4 + 2]), stof(matrix[i * 4 + 3]));
	}
	bone.mMatLocal.setColumn(0, row[0]);
	bone.mMatLocal.setColumn(1, row[1]);
	bone.mMatLocal.setColumn(2, row[2]);
	bone.mMatLocal.setColumn(3, row[3]);

	if (_parentBone == nullptr)
	{
		mRootBone = &bone;
		bone.mMatWorld = bone.mMatLocal;
		bone.mMatWorldInv = bone.mMatLocal;
		bone.mMatWorldInv.invert();
	}
	else
	{
		_parentBone->mChildBones.push_back(&bone);
		bone.mMatWorld = bone.mMatLocal * _parentBone->mMatWorld;
		bone.mMatWorldInv = bone.mMatWorld;
		bone.mMatWorldInv.invert();
	}

	XMLElement * eleNode = _ele->FirstChildElement("node");
	while (true)
	{
		if (eleNode == nullptr || strcmp(eleNode->Name(), "node"))
			break;

		LinkTreeNodes(eleNode, &bone);
		eleNode = eleNode->NextSiblingElement();
	}
}

int jBoneTree::SerchBoneIndex(string _name)
{
	int cnt = mVecBones.size();
	for (int i = 0; i < cnt; ++i)
		if (mVecBones[i].mName == _name)
			return i;
	return -1;
}

void jBoneTree::Animate(float _time, vector<Matrix4>& _rets)
{
	if (mRootBone == nullptr)
		return;

	_rets.clear();
	_rets.resize(mVecBones.size());
	mRootBone->AnimateOfAllChild(_time);

	int cnt = mVecBones.size();
	for (int i = 0; i < cnt; ++i)
		_rets[i] = mVecBones[i].mMatWorldInv * mVecBones[i].mMatAnim;

}

void jBoneTree::SetAnimate(string animName)
{
	for (jBoneNode& bone : mVecBones)
		bone.SetAnimate(animName);
}
