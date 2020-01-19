#pragma once

#include "Vectors.h"
#include "Matrices.h"

class jLine3D
{
public:
	jLine3D(Vector3 _pt, Vector3 _dir)
	{
		mPoint = _pt;
		mDirect = _dir.normalize();
	}
	~jLine3D() {}

	Vector3 GetXY(double _z);
	Vector3 GetYZ(double _x);
	Vector3 GetXZ(double _y);
	Vector3 ClosePoint(Vector3 _pt);
	static Matrix4 ProjMatGround(Vector3 dir, double _z);

private:
	Vector3 mPoint;
	Vector3 mDirect;
};

