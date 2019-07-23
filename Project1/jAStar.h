#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include "Vectors.h"
using namespace std;

typedef unsigned long			u32;
typedef unsigned long long	u64;


class jHeightMap;

enum StatePoint
{ NONE, WAIT, DONE, IMPOSSIBLE };

struct RouteInfo
{
	float weight;
	float height;
	StatePoint state;
	u64 key;
	vector<u64> fromPoints;
};

class jAStar
{
private:
	unordered_map<u64, RouteInfo> mIndexTable;
	multimap<float, u64> mWeights;
	vector<u64> mRouteResult;
	jHeightMap * mHeightMap;
	Vector2f mTargetPos;

public:
	jAStar();
	~jAStar();

	void SetHeightMap(jHeightMap* _pMap) { mHeightMap = _pMap; }
	bool Route(Vector2f _startPos, Vector2f _endPos);
	bool UpdateWeights(u64 idx);
	int FindNextKey();
	float CalcWeight(u64 idx);
	RouteInfo* GetRouteInfo(u64 idx);

private:

};

