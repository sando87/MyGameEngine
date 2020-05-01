#pragma once

#include "Vectors.h"
#include "Matrices.h"
#include <vector>

class jLine3D
{
public:
	jLine3D(Vector3 startPos, Vector3 endPos)
	{
		mPoint = startPos;
		mEndPoint = endPos;
		mDirect = endPos - startPos;
		mDirect.normalize();
	}
	~jLine3D() {}

	Vector3 GetXY(double _z);
	Vector3 GetYZ(double _x);
	Vector3 GetXZ(double _y);
	Vector3 ClosePoint(Vector3 _pt);
	bool IsContains(Vector3 _pt, double margin = 0);
	double Distance(jLine3D line, Vector3& onPointA, Vector3& onPointB);
	void ClipGrid(double step, std::vector<Vector3>& points);
	static Matrix4 ProjMatGround(Vector3 dir, double _z);

private:
	Vector3 mPoint;
	Vector3 mEndPoint;
	Vector3 mDirect;
};

