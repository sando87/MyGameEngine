#include "jAStar.h"


jAStar::jAStar()
{
	Reset();
}


jAStar::~jAStar()
{
}

bool jAStar::Route(Vector2 startWorldPos, Vector2 endWorldPos, u32 limitDepth, double step)
{
	if (mRunning)
		return false;

	Reset();

	mStep = step;
	mStartPos = startWorldPos;
	mEndPos = endWorldPos;
	u64 endKey = ToIndex(mEndPos);
	u64 curIdx = ToIndex(mStartPos);
	u32 depth = 0;
	mRunning = true;
	while (mRunning)
	{
		depth++;
		UpdateWeights(curIdx);
		curIdx = FindNextKey();
		if (curIdx == (u64)-1 || depth >= limitDepth)
			return false;
		else if (curIdx == endKey)
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
				pInfo->fromKey = idx;
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

RouteInfo * jAStar::GetRouteInfo(u64 idx)
{
	if (!Moveable(ToWorld(idx)))
		return nullptr;

	if (mIndexTable.find(idx) == mIndexTable.end())
	{
		RouteInfo info = { 0, };
		Vector2 curPos = ToWorld(idx);
		info.weight = curPos.distance(mEndPos);
		info.key = idx;
		info.state = StatePoint::NEW;
		info.fromKey = -1;
		mIndexTable[idx] = info;
	}

	RouteInfo& info = mIndexTable[idx];
	return &info;
}

void jAStar::SearchRouteResult(Vector2 startPos, double detectRange)
{
	mRouteResults.clear();
	mRouteResults.push_back(mEndPos);
	u64 nextKey = ToIndex(mEndPos);
	while (true)
	{
		nextKey = mIndexTable[nextKey].fromKey;
		if (nextKey == (u64)-1)
			break;

		Vector2 curPos = ToWorld(nextKey);
		if (curPos.distance(startPos) <= detectRange)
			break;

		mRouteResults.push_back(ToWorld(nextKey));
	}
	mRouteResults.push_back(startPos);
}

void jAStar::Reset()
{
	mIndexTable.clear();
	mWeights.clear();
	mRunning = false;
	mStep = 0;
}

//u64 jAStar::TrackBack(u64 targetIdx, u64 curIdx)
//{
//	int GridX = ToU32x(curIdx);
//	int GridY = ToU32y(curIdx);
//	u64 retKey = -1;
//	float minDist = -1;
//	for (int y = GridY - 1; y < GridY + 2; ++y)
//	{
//		if (y < 0) continue;
//		for (int x = GridX - 1; x < GridX + 2; ++x)
//		{
//			if (x < 0) continue;
//			u64 key = ToU64(x, y);
//			if (key == curIdx)
//				continue;
//			if (mIndexTable.find(key) == mIndexTable.end())
//				continue;
//			if (mIndexTable[key].state != StatePoint::CHECKED)
//				continue;
//
//			float dist = CalcDistance(key, targetIdx);
//			if (minDist < 0 || dist < minDist)
//			{
//				retKey = key;
//				minDist = dist;
//			}
//		}
//	}
//
//	if(mIndexTable.find(retKey) != mIndexTable.end())
//		mIndexTable[retKey].state = StatePoint::DONE;
//
//	return retKey;
//}

