#pragma once
#include "Vectors.h"
#include <vector>

class jRectangle2D
{
public:
	jRectangle2D();
	~jRectangle2D();
	jRectangle2D(double x, double y, double w, double h);

	void SetMinMax(Vector2 min, Vector2 max);
	void SetPosSize(Vector2 pos, Vector2 size);
	void SetCenter(Vector2 pos);
	void Zoom(double rate);
	bool Contains(Vector2 point);
	jRectangle2D Intersection(jRectangle2D & rect);
	jRectangle2D ClipRate(float rateL, float rateR, float rateT, float rateB);
	bool IsOverlapped(jRectangle2D rect);
	jRectangle2D& Expand(jRectangle2D& rect);
	jRectangle2D& Expand(Vector2 pt);
	void ClipMinus();
	void ClipGrid(double step, std::vector<Vector2>& points);

	Vector2 GetMin() { return mMin; }
	Vector2 GetMax() { return mMax; }
	Vector2 GetSize() { return mMax - mMin; }
	Vector2 GetPos() { return mMin; }
	Vector2 GetCenter() { return mMin + (GetSize() / 2); }

	//double Top() { return mMax.y; }
	//double Bottom() { return mMin.y; }
	//double Left() { return mMin.x; }
	//double Right() { return mMax.x; }
	double Width() { return mMax.x - mMin.x; }
	double Height() { return mMax.y - mMin.y; }

	jRectangle2D& operator+(Vector2 pos) { mMin += pos; mMax += pos; return (*this); }
	jRectangle2D& operator-(Vector2 pos) { mMin -= pos; mMax -= pos; return (*this); }

private:
	Vector2 mMin;
	Vector2 mMax;
};

