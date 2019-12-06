#pragma once

#include "Vectors.h"

class jRect
{
public:
	jRect() : mInit(false) {}
	jRect(Vector2 pos, Vector2 size) : mLeftBottom(pos), mRightTop(Vector2(pos.x + size.x, pos.y + size.y)), mInit(true) {}
	jRect(double left, double bottom, double right, double top) : mLeftBottom(left, bottom), mRightTop(right, top), mInit(true) {}
	~jRect() {}

private:
	Vector2 mLeftBottom;
	Vector2 mRightTop;
	bool mInit;

public:
	void SetRect(Vector2 pos, Vector2 size) { mLeftBottom = pos; mRightTop = pos + size; }
	Vector2 Center() { return Vector2((mLeftBottom.x + mRightTop.x) * 0.5, (mLeftBottom.y + mRightTop.y) * 0.5); }
	Vector2 Pos() { return mLeftBottom; }
	Vector2 Size() { return Vector2(mRightTop.x - mLeftBottom.x, mRightTop.y - mLeftBottom.y); }
	double Left() { return mLeftBottom.x; }
	double Right() { return mRightTop.x; }
	double Bottom() { return mLeftBottom.y; }
	double Top() { return mRightTop.y; }

	jRect& const expand(jRect& rect)
	{
		expand(Vector2(rect.Left(), rect.Bottom()));
		expand(Vector2(rect.Right(), rect.Top()));
		return *this;
	}
	jRect& const expand(Vector2 pt)
	{
		if (!mInit)
		{
			mInit = true;
			mLeftBottom = pt;
			mRightTop = pt;
		}
		else
		{
			mLeftBottom.x = pt.x < Left() ? pt.x : mLeftBottom.x;
			mLeftBottom.y = pt.y < Bottom() ? pt.y : mLeftBottom.y;
			mRightTop.x = pt.x > Right() ? pt.x : mRightTop.x;
			mRightTop.y = pt.y > Top() ? pt.y : mRightTop.y;
		}
		
		return *this;
	}
	bool Contains(Vector2 pt)
	{
		return Left() <= pt.x && pt.x <= Right() && Bottom() <= pt.y && pt.y <= Top();
	}
	bool Overlapped(jRect& rect)
	{
		double gapWidth = abs(Center().x - rect.Center().x);
		double gapHeight = abs(Center().y - rect.Center().y);
		double refWidth = (Size().x + rect.Size().x) * 0.5;
		double refHeight = (Size().y + rect.Size().y) * 0.5;
		return gapWidth <= refWidth && gapHeight <= refHeight;
	}


};

