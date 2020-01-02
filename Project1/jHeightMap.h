#pragma once
#include "junks.h"
class jMesh;

class jHeightMap
{
private:
	int mWidth;
	int mHeight;
	int mStep;
	int mCntWidth;
	int mCntHeight;
	vector<float> mHeightMap;
	float mMinZ;
	float mMaxZ;

public:
	jHeightMap(int _w, int _h, int _s) : mWidth(_w), mHeight(_h), mStep(_s) , mMinZ(0), mMaxZ(0)
	{
		mCntWidth = mWidth / mStep;
		mCntHeight = mHeight / mStep;
	}
	~jHeightMap();

	bool UpdateHeightMap(jMesh* _mesh, Vector3 _base);
	int IdxOfNearPt(float x, float y);
	void SetHeight(float x, float y, float z);
	float GetHeightOfPos(float x, float y);
	float GetHeightOfIdx(int idxX, int idxY);
	Vector2f GetPos(int idxX, int idxY);

	int ToIndex(int x, int y) {	return mCntWidth * y + x;	}
	int ToGridX(int idx) { return idx % mCntWidth; }
	int ToGridY(int idx) { return idx / mCntWidth; }
	float MinHeight() { return mMinZ; }
	float MaxHeight() { return mMaxZ; }
};

