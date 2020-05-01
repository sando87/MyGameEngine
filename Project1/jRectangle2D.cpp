#include "jRectangle2D.h"



jRectangle2D::jRectangle2D()
{
}


jRectangle2D::~jRectangle2D()
{
}

jRectangle2D::jRectangle2D(double x, double y, double w, double h)
{
	SetPosSize(Vector2(x, y), Vector2(w, h));
}

void jRectangle2D::SetMinMax(Vector2 min, Vector2 max)
{
	mMin = min;
	mMax = max;
	mMax.x = fmax(mMin.x, mMax.x);
	mMax.y = fmax(mMin.y, mMax.y);
}

void jRectangle2D::SetPosSize(Vector2 pos, Vector2 size)
{
	mMin = pos;
	mMax = pos + size;
	mMax.x = fmax(mMin.x, mMax.x);
	mMax.y = fmax(mMin.y, mMax.y);
}

void jRectangle2D::SetCenter(Vector2 pos)
{
	Vector2 halfSize = GetSize() / 2;
	mMin = pos - halfSize;
	mMax = pos + halfSize;
	mMax.x = fmax(mMin.x, mMax.x);
	mMax.y = fmax(mMin.y, mMax.y);
}

void jRectangle2D::Zoom(double rate)
{
	Vector2 sized = GetSize() * rate;
	SetPosSize(GetPos(), sized);
}

bool jRectangle2D::Contains(Vector2 point)
{
	return mMin.x <= point.x && point.x < mMax.x && mMin.y <= point.y && point.y < mMax.y;
}

jRectangle2D jRectangle2D::Intersection(jRectangle2D & rect)
{
	jRectangle2D inter;
	double top = fmin(mMax.y, rect.GetMax().y);
	double bottom = fmax(mMin.y, rect.GetMin().y);
	double left = fmax(mMin.x, rect.GetMin().x);
	double right = fmin(mMax.x, rect.GetMax().x);
	if (left > right) left = right;
	if (bottom > top) bottom = top;
	inter.SetMinMax(Vector2(left, bottom), Vector2(right, top));
	return inter;
}

jRectangle2D jRectangle2D::ClipRate(float rateL, float rateR, float rateT, float rateB)
{
	double left = mMin.x + (Width() * rateL);
	left = left > mMax.x ? mMax.x : left;
	double right = mMax.x - (Width() * rateR);
	right = right < mMin.x ? mMin.x : right;
	double top = mMin.y + (Height() * rateT);
	top = top > mMax.y ? mMax.y : top;
	double bottom = mMax.y - (Height() * rateB);
	bottom = bottom < mMin.y ? mMin.y : bottom;
	return jRectangle2D(left, top, right - left, bottom - top);
}

bool jRectangle2D::IsOverlapped(jRectangle2D rect)
{
	double gapWidth = abs(GetCenter().x - rect.GetCenter().x);
	double gapHeight = abs(GetCenter().y - rect.GetCenter().y);
	double refWidth = (GetSize().x + rect.GetSize().x) * 0.5;
	double refHeight = (GetSize().y + rect.GetSize().y) * 0.5;
	return gapWidth <= refWidth && gapHeight <= refHeight;
}

jRectangle2D & jRectangle2D::Expand(jRectangle2D & rect)
{
	Expand(rect.GetMin());
	Expand(rect.GetMax());
	return *this;
}

jRectangle2D & jRectangle2D::Expand(Vector2 pt)
{
	mMin.x = pt.x < mMin.x ? pt.x : mMin.x;
	mMin.y = pt.y < mMin.y ? pt.y : mMin.y;
	mMax.x = pt.x > mMax.x ? pt.x : mMax.x;
	mMax.y = pt.y > mMax.y ? pt.y : mMax.y;
	return *this;
}

void jRectangle2D::ClipMinus()
{
	mMin.x = mMin.x	< 0 ? 0 : mMin.x;
	mMin.y = mMin.y	< 0 ? 0 : mMin.y;
	mMax.x = mMax.x	< 0 ? 0 : mMax.x;
	mMax.y = mMax.y	< 0 ? 0 : mMax.y;
}

void jRectangle2D::ClipGrid(double step, std::vector<Vector2>& points)
{
	int n = (int)(Width() / step) + 1;
	double clipStep = Width() / n;
	for (int y = 0; y <= n; y++)
	{
		for (int x = 0; x <= n; x++)
		{
			Vector2 point = GetMin() + Vector2(x * clipStep, y * clipStep);
			points.push_back(point);
		}
	}
}
