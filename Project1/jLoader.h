#pragma once

#include "junks.h"

class jLoader
{
public:
	jLoader() {}
	~jLoader() { Release(); }

	vector<string> mInfo;
	vector<Vector3f> mPos;
	vector<Vector2f> mUV;
	vector<Vector3f> mNormal;
	vector<Vector4n> mBoneIndexs;
	vector<Vector4f> mWeights;
	vector<Vector3n> mVertexIdx;

	void LoadObjFile(string _filename);
	void LoadDaeFile(string _filename);

	void Release()
	{
		mInfo.clear();
		mPos.clear();
		mUV.clear();
		mNormal.clear();
		mBoneIndexs.clear();
		mWeights.clear();
		mVertexIdx.clear();
	}
};

