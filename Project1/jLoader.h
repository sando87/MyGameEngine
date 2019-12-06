#pragma once

#include <vector>
using namespace std;

#include "header.h"
#include "Vectors.h"
#include "TmpComStruct.h"

class jLoader
{
public:
	jLoader() {}
	~jLoader() { Release(); }

	vector<string> mInfo;
	vector<Vector3f> mPos;
	vector<Vector2f> mUV;
	vector<Vector3f> mNormal;
	vector<vector<Vector3n>> mFaceInfo;
	vector<BoneWeights> mWeightPos;

	void LoadObjFile(string _filename);
	void LoadObjFile2(string _filename);
	void LoadDaeFile(string _filename);

	void Release()
	{
		mInfo.clear();
		mPos.clear();
		mUV.clear();
		mNormal.clear();
		mFaceInfo.clear();
	}
};

