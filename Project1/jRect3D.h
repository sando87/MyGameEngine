#pragma once

#include "Vectors.h"
#include "jRect.h"
#include "jLine3D.h"
#include <vector>
using namespace std;

class jRect3D
{
public:
	jRect3D() : mInit(false) {}
	jRect3D(Vector3 pos, Vector3 size) : mMinXYZ(pos), mMaxXYZ(Vector3(pos + size)), mInit(true) {}
	//jRect3D(Vector3 min, Vector3 max) : mMinXYZ(min), mMaxXYZ(max), mInit(true) {}
	~jRect3D() {}

private:
	Vector3 mMinXYZ;
	Vector3 mMaxXYZ;
	bool mInit;

public:
	void SetRect(Vector3 pos, Vector3 size) { mMinXYZ = pos; mMaxXYZ = pos + size; }
	Vector3 Center() { return (mMinXYZ + mMaxXYZ) * 0.5; }
	Vector3 Pos() { return mMinXYZ; }
	Vector3 Size() { return mMaxXYZ - mMinXYZ; }
	jRect LeftRight() { return jRect(mMinXYZ.y, mMinXYZ.z, mMaxXYZ.y, mMaxXYZ.z); }
	jRect TopBottom() { return jRect(mMinXYZ.x, mMinXYZ.y, mMaxXYZ.x, mMaxXYZ.y); }
	jRect FrontBack() { return jRect(mMinXYZ.x, mMinXYZ.z, mMaxXYZ.x, mMaxXYZ.z); }

	bool PassedBy(jLine3D line, vector<Vector3>& vecs);
	jRect3D& const expand(Vector3 pt);
	jRect3D& const expand(jRect3D rect);
};

