#pragma once
#include "Vectors.h"
class jModel;

class jHeightMap
{
private:
	int mWidth;
	int mHeight;
	int mStep;
	int mCntWidth;
	int mCntHeight;
	float *mHeightMap;

public:
	jHeightMap(int _w, int _h, int _s) : mWidth(_w), mHeight(_h), mStep(_s) 
	{
		mCntWidth = mWidth / mStep;
		mCntHeight = mHeight / mStep;
		mHeightMap = nullptr;
	}
	~jHeightMap();

	bool CreateHeightMap(jModel* _mesh);
	int IdxOfNearPt(float x, float y);
	void SetHeight(float x, float y, float z);
	float GetHeightOfPos(float x, float y);
	float GetHeightOfIdx(int idxX, int idxY);
	Vector2f GetPos(int idxX, int idxY);

	int ToIndex(int x, int y) {	return mCntWidth * y + x;	}
	int ToGridX(int idx) { return idx % mCntWidth; }
	int ToGridY(int idx) { return idx / mCntWidth; }

};

