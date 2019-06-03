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

private:
	Position3 mPoint;
	Vector3 mDirect;
};
