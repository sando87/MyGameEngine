#include "jLine3D.h"
#include "junks.h"

Vector3 jLine3D::GetXY(double _z)
{
	double d = mDirect.z / (_z - mPoint.z);
	double x = (mDirect.x / d) + mPoint.x;
	double y = (mDirect.y / d) + mPoint.y;
	return Vector3(x, y, _z);
}
Vector3 jLine3D::GetYZ(double _x)
{
	double d = mDirect.x / (_x - mPoint.x);
	double z = (mDirect.z / d) + mPoint.z;
	double y = (mDirect.y / d) + mPoint.y;
	return Vector3(_x, y, z);
}
Vector3 jLine3D::GetXZ(double _y)
{
	double d = mDirect.y / (_y - mPoint.y);
	double x = (mDirect.x / d) + mPoint.x;
	double z = (mDirect.z / d) + mPoint.z;
	return Vector3(x, _y, z);
}

Vector3 jLine3D::ClosePoint(Vector3 _pt)
{
	double dot = mDirect.dot(_pt - mPoint);
	return mDirect * dot + mPoint;
}

bool jLine3D::IsContains(Vector3 _pt, double margin)
{
	double dotA = (mEndPoint - mPoint).dot(mDirect);
	double dotB = (_pt - mPoint).dot(mDirect);
	if (dotA > 0)
		return -margin < dotB && dotB < dotA + margin;
	else
		return dotA - margin < dotB && dotB < margin;
}

double jLine3D::Distance(jLine3D line, Vector3& onPointA, Vector3& onPointB)
{
	double a = mDirect.dot(line.mDirect);
	if (abs(1 - a) < 0.0001) //in case of parallel
	{
		double k = (mPoint - line.mPoint).dot(line.mDirect);
		onPointA = mPoint;
		onPointB = k * line.mDirect + line.mPoint;
		return onPointA.distance(onPointB);
	}

	double b = (mPoint - line.mPoint).dot(line.mDirect);
	double c = (line.mPoint - mPoint).dot(mDirect);
	double d = (1 - a * a);
	double k1 = (b * a + c) / d;
	double k2 = a * k1 + b;
	onPointA = k1 * mDirect + mPoint;
	onPointB = k2 * line.mDirect + line.mPoint;
	return onPointA.distance(onPointB);

	//For test calculating distance of lines
	//double k1 = 1.0;
	//Vector3 Pk1 = mDirect * k1 + mPoint;
	//Vector3 P2Pk1 = Pk1 - line.mPoint;
	//double k2 = P2Pk1.dot(line.mDirect);
	//Vector3 Pk2 = line.mDirect * k2 + line.mPoint;
	//Vector3 Pk2Pk1 = Pk1 - Pk2;
	//double retDot = Pk2Pk1.dot(mDirect);
}

void jLine3D::ClipGrid(double step, std::vector<Vector3>& points)
{
	double dist = Vector2(mPoint).distance(Vector2(mEndPoint));
	int n = (int)(dist / step) + 1;
	Vector3 dir = mEndPoint - mPoint;
	double clipStep = dir.length() / n;
	for (int x = 0; x <= n; x++)
		points.push_back(mPoint + mDirect * x * clipStep);
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
