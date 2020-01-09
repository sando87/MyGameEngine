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

Vector3 jLine3D::ClosePoint(Vector3 _pt)
{
	double dot = mDirect.dot(_pt - mPoint);
	return mDirect * dot + mPoint;
}

Matrix4 jLine3D::ProjMatGround(Vector3 dir, double _z)
{
	dir.normalize();
	Matrix4 mat;
	mat.identity();
	mat[0] = 1;
	mat[5] = 1;
	mat[8] = -dir.x / dir.z;
	mat[9] = -dir.y / dir.z;
	mat[10] = 0;
	mat[12] = dir.x * _z / dir.z;
	mat[13] = dir.y * _z / dir.z;
	mat[14] = _z;
	mat[15] = 1;
	return mat;
}
