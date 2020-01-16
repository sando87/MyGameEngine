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
	u64 curIdx = mStartKey;
	u32 depth = 0;
	mRunning = true;
	while (mRunning)
	{
		depth++;
		UpdateWeights(curIdx);
		curIdx = FindNextKey();
		if (curIdx == (u64)-1 || depth >= limitDepth)
			return false;
		else if (curIdx == mEndKey)
			break;
	}
	mRunning = false;
	return true;
}

bool jAStar::UpdateWeights(u64 idx)
{
	int GridX = ToU32x(idx);
	int GridY = ToU32y(idx);
	for (int y = GridY - 1; y < GridY + 2; ++y)
	{
		if (y < 0) continue;
		for (int x = GridX - 1; x < GridX + 2; ++x)
		{
			if (x < 0) continue;
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

u64 jAStar::FindNextKey()
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

void jAStar::SearchRouteResult(u64 targetIdx)
{
	mRouteResults.clear();
	if (mIndexTable.find(targetIdx) == mIndexTable.end())
	{
		_warn();
		return;
	}
	
	mRouteResults.push_back(mEndKey);
	u64 nextKey = mEndKey;
	while (true)
	{
		nextKey = TrackBack(targetIdx, nextKey);
		if (nextKey == -1)
			break;
		if (nextKey == targetIdx)
			break;

		mRouteResults.push_back(nextKey);
	}
}

u64 jAStar::TrackBack(u64 targetIdx, u64 curIdx)
{
	int GridX = ToU32x(curIdx);
	int GridY = ToU32y(curIdx);
	u64 retKey = -1;
	float minDist = -1;
	for (int y = GridY - 1; y < GridY + 2; ++y)
	{
		if (y < 0) continue;
		for (int x = GridX - 1; x < GridX + 2; ++x)
		{
			if (x < 0) continue;
			u64 key = ToU64(x, y);
			if (key == curIdx)
				continue;
			if (mIndexTable.find(key) == mIndexTable.end())
				continue;
			if (mIndexTable[key].state != StatePoint::CHECKED)
				continue;

			float dist = CalcDistance(key, targetIdx);
			if (minDist < 0 || dist < minDist)
			{
				retKey = key;
				minDist = dist;
			}
		}
	}

	if(mIndexTable.find(retKey) != mIndexTable.end())
		mIndexTable[retKey].state = StatePoint::DONE;

	return retKey;
}

void jAStar::Reset()
{
	mIndexTable.clear();
	mWeights.clear();
	mStartKey = 0;
	mEndKey = 0;
	mRunning = false;
}

