#include "jLine3D.h"

Position2 jLine3D::GetXY(double _z)
{
	double d = mDirect.z / (_z - mPoint.z);
	double x = (mDirect.x / d) + mPoint.x;
	double y = (mDirect.y / d) + mPoint.y;
	return Position2(x, y);
}
Position2 jLine3D::GetYZ(double _x)
{
	double d = mDirect.x / (_x - mPoint.x);
	double z = (mDirect.z / d) + mPoint.z;
	double y = (mDirect.y / d) + mPoint.y;
	return Position2(y, z);
}
Position2 jLine3D::GetXZ(double _y)
{
	double d = mDirect.y / (_y - mPoint.y);
	double x = (mDirect.x / d) + mPoint.x;
	double z = (mDirect.z / d) + mPoint.z;
	return Position2(x, z);
}