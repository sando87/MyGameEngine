#include "jRect3D.h"

bool jRect3D::IsContains(Vector3 pos)
{
	if (pos.x < mMinXYZ.x || mMaxXYZ.x < pos.x)
		return false;
	if (pos.y < mMinXYZ.y || mMaxXYZ.y < pos.y)
		return false;
	if (pos.z < mMinXYZ.z || mMaxXYZ.z < pos.z)
		return false;
	return true;
}

bool jRect3D::PassedBy(jLine3D line, vector<Vector3>& vecs)
{
	Vector2 ptLeft = line.GetYZ(mMinXYZ.x);
	if (Side().Contains(ptLeft))	vecs.push_back(Vector3(mMinXYZ.x, ptLeft.x, ptLeft.y));
	Vector2 ptRight = line.GetYZ(mMaxXYZ.x);
	if (Side().Contains(ptRight))	vecs.push_back(Vector3(mMaxXYZ.x, ptRight.x, ptRight.y));
	Vector2 ptFront = line.GetXZ(mMinXYZ.y);
	if (Front().Contains(ptFront))	vecs.push_back(Vector3(ptFront.x, mMinXYZ.y, ptFront.y));
	Vector2 ptBack = line.GetXZ(mMaxXYZ.y);
	if (Front().Contains(ptBack))	vecs.push_back(Vector3(ptBack.x, mMaxXYZ.y, ptBack.y));
	Vector2 ptBottom = line.GetXY(mMinXYZ.z);
	if (Top().Contains(ptBottom))	vecs.push_back(Vector3(ptBottom.x, ptBottom.y, mMinXYZ.z));
	Vector2 ptTop = line.GetXY(mMaxXYZ.z);
	if (Top().Contains(ptTop))	vecs.push_back(Vector3(ptTop.x, ptTop.y, mMaxXYZ.z));
	return vecs.size() > 0 ? true : false;
}
jRect3D& jRect3D::expand(Vector3 pt)
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

jRect3D& jRect3D::expand(jRect3D rect)
{
	expand(rect.Pos());
	expand(rect.Pos() + rect.Size());
	return *this;
}