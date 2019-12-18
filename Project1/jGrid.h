#pragma once

#include <vector>
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

	//void Set(double posX, double posY, Ty& val) { mVecs[ToKey(posX, posY)] = val; }
	Ty& Get(Vector2 pos) { return mVecs[ToKey(pos)]; }
	void GetGrids(jRect rect, std::vector<Ty*>& rets) 
	{
		for (double y = rect.Bottom(); y < rect.Top(); y += mStep)
			for (double x = rect.Left(); x < rect.Right(); x += mStep)
				rets.push_back(&mVecs[ToKey(Vector2(x, y))]);
	}
	//unsigned int ToKey(int idxX, int idxY) { return (idxY * mTotalIndex) + idxX; }
	unsigned int ToKey(Vector2 pos) { return ((int)(pos.y / mStep) * mTotalIndex) + (int)(pos.x / mStep); }
private:
	double mStep;
	int mTotalIndex;
	std::vector<Ty> mVecs;
	
};

