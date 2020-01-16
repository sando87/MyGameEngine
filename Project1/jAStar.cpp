#include "jAStar.h"


jAStar::jAStar()
{
	Reset();
}


jAStar::~jAStar()
{
}

bool jAStar::Route(u32 startIdxX, u32 startIdx, u32 endIdxX, u32 endIdxY, u32 limitDepth)
{
	if (mRunning)
		return false;

	Reset();

	mStartKey = ToU64(startIdxX, startIdx);
	mEndKey = ToU64(endIdxX, endIdxY);
	int curIdx = startIdx;
	u32 depth = 0;
	mRunning = true;
	while (mRunning)
	{
		depth++;
		UpdateWeights(curIdx);
		curIdx = FindNextKey();
		if (curIdx == -1 || depth >= limitDepth)
			return false;
		else if (curIdx == mEndKey)
			break;
	}
	SearchRouteResult();
	mRunning = false;
	return true;
}

bool jAStar::UpdateWeights(u64 idx)
{
	int GridX = ToU32x(idx);
	int GridY = ToU32y(idx);
	for (int y = GridY - 1; y < GridY + 2; ++y)
	{
		for (int x = GridX - 1; x < GridX + 2; ++x)
		{
			u64 key = ToU64(x, y);
			RouteInfo* pInfo = GetRouteInfo(key);
			if (pInfo != nullptr && pInfo->state == StatePoint::NEW)
			{
				pInfo->state = StatePoint::WAIT;
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
		mWeights.erase(iter++);
		RouteInfo& info = mIndexTable[key];
		if (info.state == StatePoint::WAIT)
		{
			info.state = StatePoint::CHECKED;
			return info.key;
		}
	}
	return -1;
}

float jAStar::CalcDistance(u64 idxA, u64 idxB)
{
	int dx = ToU32x(idxA) - ToU32x(idxB);
	int dy = ToU32y(idxA) - ToU32y(idxB);
	return sqrtf(dx * dx + dy * dy);
}

RouteInfo * jAStar::GetRouteInfo(u64 idx)
{
	if (!Moveable(ToU32x(idx), ToU32y(idx)))
		return nullptr;

	if (mIndexTable.find(idx) == mIndexTable.end())
	{
		RouteInfo info = { 0, };
		info.weight = CalcDistance(idx, mEndKey);
		info.key = idx;
		info.state = StatePoint::NEW;
		mIndexTable[idx] = info;
	}

	RouteInfo& info = mIndexTable[idx];
	return &info;
}

void jAStar::SearchRouteResult()
{
	mRouteResult.clear();
	mRouteResult.push_back(mEndKey);
	TrackBack(mEndKey);
}

void jAStar::TrackBack(u64 targetIdx)
{
	int GridX = ToU32x(targetIdx);
	int GridY = ToU32y(targetIdx);
	u64 retKey = 0;
	float minDist = -1;
	for (int y = GridY - 1; y < GridY + 2; ++y)
	{
		for (int x = GridX - 1; x < GridX + 2; ++x)
		{
			u64 key = ToU64(x, y);
			if (key == targetIdx)
				continue;
			if (mIndexTable.find(key) == mIndexTable.end())
				continue;
			if (mIndexTable[key].state != StatePoint::CHECKED)
				continue;

			float dist = CalcDistance(key, mStartKey);
			if (minDist < 0 || dist < minDist)
			{
				retKey = key;
				minDist = dist;
			}
		}
	}
	mIndexTable[retKey].state = StatePoint::DONE;
	mRouteResult.push_back(retKey);
	if (retKey != mStartKey)
		TrackBack(retKey);
	return;
}

void jAStar::Reset()
{
	mIndexTable.clear();
	mWeights.clear();
	mRouteResult.clear();
	mStartKey = 0;
	mEndKey = 0;
	mRunning = false;
}

