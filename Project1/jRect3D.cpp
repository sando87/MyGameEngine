#include "jRect3D.h"

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

bool jRect3D::PassedBy(jLine3D line, vector<Vector3>& vecs)
{
	Vector2 ptLeft = line.GetYZ(mMinXYZ.x);
	if (LeftRight().Contains(ptLeft))	vecs.push_back(Vector3(mMinXYZ.x, ptLeft.x, ptLeft.y));
	Vector2 ptRight = line.GetYZ(mMaxXYZ.x);
	if (LeftRight().Contains(ptRight))	vecs.push_back(Vector3(mMaxXYZ.x, ptRight.x, ptRight.y));
	Vector2 ptFront = line.GetXZ(mMinXYZ.y);
	if (FrontBack().Contains(ptFront))	vecs.push_back(Vector3(ptFront.x, mMinXYZ.y, ptFront.y));
	Vector2 ptBack = line.GetXZ(mMaxXYZ.y);
	if (FrontBack().Contains(ptBack))	vecs.push_back(Vector3(ptBack.x, mMaxXYZ.y, ptBack.y));
	Vector2 ptBottom = line.GetXY(mMinXYZ.z);
	if (TopBottom().Contains(ptBottom))	vecs.push_back(Vector3(ptBottom.x, ptBottom.y, mMinXYZ.z));
	Vector2 ptTop = line.GetXY(mMaxXYZ.z);
	if (TopBottom().Contains(ptTop))	vecs.push_back(Vector3(ptTop.x, ptTop.y, mMaxXYZ.z));
	return vecs.size() > 0 ? true : false;
}
jRect3D& const jRect3D::expand(Vector3 pt)
{
	if (!mInit)
	{
		mInit = true;
		mMinXYZ = pt;
		mMaxXYZ = pt;
	}
	else
	{
		mMinXYZ.x = min(pt.x, mMinXYZ.x);
		mMinXYZ.y = min(pt.y, mMinXYZ.y);
		mMinXYZ.z = min(pt.z, mMinXYZ.z);
		mMaxXYZ.x = max(pt.x, mMaxXYZ.x);
		mMaxXYZ.y = max(pt.y, mMaxXYZ.y);
		mMaxXYZ.z = max(pt.z, mMaxXYZ.z);
	}

	return *this;
}

jRect3D& const jRect3D::expand(jRect3D& rect)
{
	expand(rect.Pos());
	expand(rect.Pos() + rect.Size());
	return *this;
}