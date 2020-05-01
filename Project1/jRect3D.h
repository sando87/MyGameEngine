#pragma once

#include "junks.h"
#include "jRectangle2D.h"
#include "jLine3D.h"

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
	void SetCenter(Vector3 cenPos) { Vector3 sizeHalf = Size() * 0.5; mMinXYZ = cenPos - sizeHalf; mMaxXYZ = cenPos + sizeHalf; }
	void SetPos(Vector3 minPos) { Vector3 size = Size(); mMinXYZ = minPos; mMaxXYZ = minPos + size; }
	void Move(Vector3 delta) { mMaxXYZ += delta; mMinXYZ += delta; }
	Vector3 Center() { return (mMinXYZ + mMaxXYZ) * 0.5; }
	Vector3 Pos() { return mMinXYZ; }
	Vector3 Min() { return mMinXYZ; }
	Vector3 Max() { return mMaxXYZ; }
	Vector3 Size() { return mMaxXYZ - mMinXYZ; }
	jRectangle2D Side() { return jRectangle2D(mMinXYZ.y, mMinXYZ.z, mMaxXYZ.y - mMinXYZ.y, mMaxXYZ.z - mMinXYZ.z); }
	jRectangle2D Top() { return jRectangle2D(mMinXYZ.x, mMinXYZ.y, mMaxXYZ.x - mMinXYZ.x, mMaxXYZ.y - mMinXYZ.y); }
	jRectangle2D Front() { return jRectangle2D(mMinXYZ.x, mMinXYZ.z, mMaxXYZ.x - mMinXYZ.x, mMaxXYZ.z - mMinXYZ.z); }

	bool IsContains(Vector3 pos);
	bool PassedBy(jLine3D line, vector<Vector3>& vecs);
	jRect3D& expand(Vector3 pt);
	jRect3D& expand(jRect3D rect);

	jRect3D& operator+(Vector3 pos) { mMinXYZ += pos; mMaxXYZ += pos; return (*this); }
	jRect3D& operator-(Vector3 pos) { mMinXYZ -= pos; mMaxXYZ -= pos; return (*this); }
};

