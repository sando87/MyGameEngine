#pragma once

#include "junks.h"

enum StatePoint
{ NEW, WAIT, CHECKED, DONE };

struct RouteInfo
{
	float weight;
	StatePoint state;
	u64 key;
};

class jAStar
{
private:
	unordered_map<u64, RouteInfo> mIndexTable;
	multimap<float, u64> mWeights;
	vector<u64> mRouteResults;
	u64 mStartKey;
	u64 mEndKey;
	bool mRunning;

public:
	jAStar();
	~jAStar();

	function<bool(u32, u32)> Moveable;
	void StopRouting() { mRunning = false; }
	bool Route(u32 startIdxX, u32 startIdx, u32 endIdxX, u32 endIdxY, u32 limitDepth);
	vector<u64>& GetResults() { return mRouteResults; }
	void SearchRouteResult(u64 targetIdx);

private:
	bool UpdateWeights(u64 idx);
	u64 FindNextKey();
	float CalcDistance(u64 idxA, u64 idxB);
	RouteInfo* GetRouteInfo(u64 idx);
	u64 TrackBack(u64 targetIdx, u64 curIdx);
	void Reset();

};

