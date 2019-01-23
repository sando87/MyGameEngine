#include "jLine3D.h"

Position2 jLine3D::GetXY(double _z)
{
	double d = mDirect.z / (_z - mPoint.z);
	double x = (mDirect.x / d) + mPoint.x;
	double y = (mDirect.y / d) + mPoint.y;
	return Position2(x, y);
}