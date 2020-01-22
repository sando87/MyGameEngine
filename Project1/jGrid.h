#pragma once

#include <vector>
#include <list>
#include "jRect.h"

template<typename Ty>
class jGrid
{
public:
	jGrid(double size, double step) : mStep(step), mTotalIndex(size/step) 
	{
		mVecs.clear();
		mVecs.resize(mTotalIndex * mTotalIndex);
	}
	~jGrid() {}

	Ty& Get(Vector2 pos) { return mVecs[ToKey(pos)]; }
	void GetGrids(jRect rect, std::vector<Ty*>& rets) 
	{
		for (double y = rect.Bottom(); y < rect.Top(); y += mStep)
			for (double x = rect.Left(); x < rect.Right(); x += mStep)
				rets.push_back(&mVecs[ToKey(Vector2(x, y))]);
	}
	//template<>
	//void GetGrids(jRect rect, std::list<jCrash*>& rets)
	//{
	//	for (double y = rect.Bottom(); y < rect.Top(); y += mStep)
	//		for (double x = rect.Left(); x < rect.Right(); x += mStep)
	//		{
	//			std::list<jCrash*>& info = Get(Vector2(x, y));
	//			rets.insert(rets.end(), info.begin(), info.end());
	//		}
	//}
private:
	double mStep;
	int mTotalIndex;
	std::vector<Ty> mVecs;

	unsigned int ToKey(Vector2 pos) { return ((int)(pos.y / mStep) * mTotalIndex) + (int)(pos.x / mStep); }
	
};

