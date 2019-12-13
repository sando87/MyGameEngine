#pragma once
#include "jGameObject.h"
#include "jRect3D.h"

class ObjTerrain;
class jHeightMap;

#define GRID_HASH(x, y) (u64)(((u64)(y) << 32) | (u64)(x))

struct TerrainBlock
{
	jRect3D rect;
	vector<ObjTerrain*> terrains;
	jHeightMap* heightMap;
};

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
	unordered_map<uint64_t, TerrainBlock> mCachedBlocks;
	int mBlockSize;

	void LoadTerrainGridMetaInfo();
	void LoadingBlocks();
	void ClearFarBlocks(int clearCount);
	void LoadBlock(int idxX, int idxY, TerrainBlock& block);
	//ObjTerrain* GetTerrain(float worldX, float worldY);
	u64 CoordinateToKey(float x, float y)
	{
		int nx = (x / mBlockSize);
		int ny = (y / mBlockSize);
		u64 key = GRID_HASH(nx, ny);
		return key;
	}

public:
	Vector3 CalcGroundPos(Vector3 pos, Vector3 dir);
	bool GetHeight(float worldX, float worldY, float& height);
};

