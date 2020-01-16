#pragma once
#include "jGameObject.h"
#include "jRect3D.h"

class jZMapLoader;

struct TerrainBlock
{
	vector<jGameObject*> terrains;
	jZMapLoader* zMap;
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
	Vector3 mTerrainCenter;

	void LoadTerrainGridMetaInfo();
	void LoadingBlocks();
	void ClearFarBlocks(int clearCount);
	void LoadBlock(int idxX, int idxY, TerrainBlock& block);
	//ObjTerrain* GetTerrain(float worldX, float worldY);
	u64 CoordinateToKey(float x, float y)
	{
		int nx = (int)(x / mBlockSize);
		int ny = (int)(y / mBlockSize);
		u64 key = ToU64(nx, ny);
		return key;
	}

public:
	Vector3 CalcGroundPos(Vector3 pos, Vector3 dir);
	bool GetHeight(float worldX, float worldY, float& height);
	Vector3 GetTerrainCenter() { return mTerrainCenter; }
};

