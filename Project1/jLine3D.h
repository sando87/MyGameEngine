#pragma once

#include "Vectors.h"

class jLine3D
{
public:
	jLine3D(Position3 _pt, Vector3 _dir)
	{
		mPoint = _pt;
		mDirect = _dir.normalize();
	}
	~jLine3D() {}

	Position2 GetXY(double _z);
	Position2 GetYZ(double _x);
	Position2 GetXZ(double _y);
	Vector3 ClosePoint(Vector3 _pt);

private:
	Position3 mPoint;
	Vector3 mDirect;
};

