#pragma once

#include "junks.h"

enum StatePoint
{ NEW, WAIT, CHECKED, DONE };

struct RouteInfo
{
	float weight;
	StatePoint state;
	u64 key;
	u64 fromKey;
};

class jAStar
{
private:
	unordered_map<u64, RouteInfo> mIndexTable;
	multimap<float, u64> mWeights;
	vector<Vector2> mRouteResults;
	Vector2 mStartPos;
	Vector2 mEndPos;
	bool mRunning;
	double mStep;

public:
	jAStar();
	~jAStar();

	function<bool(Vector2)> Moveable;
	void StopRouting() { mRunning = false; }
	bool Route(Vector2 startWorldPos, Vector2 endWorldPos, u32 limitDepth, double step);
	vector<Vector2>& GetResults() { return mRouteResults; }
	void SearchRouteResult(Vector2 startPos, double detectRange);

private:
	bool UpdateWeights(u64 idx);
	u64 FindNextKey();
	RouteInfo* GetRouteInfo(u64 idx);
	//u64 TrackBack(u64 targetIdx, u64 curIdx);
	void Reset();
	Vector2 ToWorld(u64 idx) { return Vector2(ToU32x(idx) * mStep, ToU32y(idx) * mStep); }
	u64 ToIndex(Vector2 pos) { return ToU64(pos.x / mStep, pos.y / mStep); }

};

