#include "jAStar.h"
#include "jHeightMap.h"


jAStar::jAStar()
{
}


jAStar::~jAStar()
{
	mIndexTable.clear();
	mWeights.clear();
	mRouteResult.clear();
	mHeightMap = nullptr;
	mTargetPos = Vector2f();
}

bool jAStar::Route(Vector2f _startPos, Vector2f _endPos)
{
	mTargetPos = _endPos;
	u64 startIdx = mHeightMap->IdxOfNearPt(_startPos.x, _startPos.y);
	u64 endIdx = mHeightMap->IdxOfNearPt(_endPos.x, _endPos.y);
	int curIdx = startIdx;
	while (true)
	{
		UpdateWeights(curIdx);
		curIdx = FindNextKey();
		int X = mHeightMap->ToGridX(curIdx);
		int Y = mHeightMap->ToGridY(curIdx);
		//printf("[%d, %d, %d]\n", X, Y, mWeights.size());
		if (curIdx == -1)
			return false;
		else if (curIdx == endIdx)
			break;
	}
	return true;
}

bool jAStar::UpdateWeights(u64 idx)
{
	RouteInfo* centerInfo = GetRouteInfo(idx);
	centerInfo->state = StatePoint::DONE;

	int GridX = mHeightMap->ToGridX(idx);
	int GridY = mHeightMap->ToGridY(idx);
	for (int y = GridY - 1; y < GridY + 2; ++y)
	{
		for (int x = GridX - 1; x < GridX + 2; ++x)
		{
			u64 key = mHeightMap->ToIndex(x, y);
			RouteInfo* pInfo = GetRouteInfo(key);
			int diff = abs(pInfo->height - centerInfo->height);
			if (diff > 2)
				pInfo->state = StatePoint::IMPOSSIBLE;

			if (pInfo->state == StatePoint::NONE)
			{
				pInfo->state = StatePoint::WAIT;
				pInfo->fromPoints.push_back(idx);
				mWeights.insert(pair<float, u64>(pInfo->weight, pInfo->key));
			}
		}
	}
	return true;
}

int jAStar::FindNextKey()
{
	for (auto iter = mWeights.begin(); iter != mWeights.end(); )
	{
		u64 key = iter->second;
		RouteInfo& info = mIndexTable[key];
		if (info.state == StatePoint::WAIT)
			return key;
		else
		{
			mWeights.erase(iter++);
		}
	}
	return -1;
}

float jAStar::CalcWeight(u64 idx)
{
	int GridX = mHeightMap->ToGridX(idx);
	int GridY = mHeightMap->ToGridY(idx);
	Vector2f pos = mHeightMap->GetPos(GridX, GridY);
	return (pos - mTargetPos).length();
}

RouteInfo * jAStar::GetRouteInfo(u64 idx)
{
	if (mIndexTable.find(idx) == mIndexTable.end())
	{
		RouteInfo info = { 0, };
		info.weight = CalcWeight(idx);
		int X = mHeightMap->ToGridX(idx);
		int Y = mHeightMap->ToGridY(idx);
		info.height = mHeightMap->GetHeightOfIdx(X, Y);
		info.key = idx;
		info.state = StatePoint::NONE;
		mIndexTable[idx] = info;
	}

	RouteInfo& info = mIndexTable[idx];
	return &info;
}

