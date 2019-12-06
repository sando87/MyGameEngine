#pragma once
#include "jGameObject.h"
#include <unordered_map>
#include "Vectors.h"
#include "jTypeDef.h"

using namespace std;

#define GRID_HASH(x, y) (u64)(((u64)(y) << 32) | (u64)(x))
class ObjTerrain;

class ObjTerrainMgr :
	public jGameObject
{
public:
	ObjTerrainMgr();
	virtual ~ObjTerrainMgr();

private:
	virtual void OnStart();
	virtual void OnUpdate();


	unordered_map<uint64_t, vector<string>*> mBlockResourcesAll;
	unordered_map<uint64_t, ObjTerrain*> mCachedBlocks;
	int mBlockSize;

	void LoadTerrainGridMetaInfo();
	void LoadingBlocks();
	void ClearFarBlocks(int clearCount);
	ObjTerrain* LoadBlock(int idxX, int idxY);
	ObjTerrain* GetTerrain(float worldX, float worldY);
	u64 CoordinateToKey(float x, float y)
	{
		int nx = (x / mBlockSize);
		int ny = (y / mBlockSize);
		u64 key = GRID_HASH(nx, ny);
		return key;
	}

public:
	Vector3 CalcGroundPos(Vector3 pos, Vector3 dir);
	float GetHeight(float worldX, float worldY);
};

