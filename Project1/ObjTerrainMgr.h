#pragma once
#include "jGameObject.h"
#include "jRect3D.h"

class jZMapLoader;
class ObjCamera;

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
	ObjCamera* mCamera;
	int mBlockSize;

	void LoadTerrainGridMetaInfo();
	void LoadingBlocks();
	void ClearFarBlocks(int clearCount);
	void LoadBlock(int idxX, int idxY, TerrainBlock& block);
	u64 CoordinateToKey(float x, float y)
	{
		int nx = (int)(x / mBlockSize);
		int ny = (int)(y / mBlockSize);
		u64 key = ToU64(nx, ny);
		return key;
	}

public:
	bool RayCastTerrain(Vector3 pos, Vector3 dir, Vector2& outPoint);
	bool Reachable(Vector2 start, Vector2 end, Vector2& lastPoint, double step);
	bool GetHeight(float worldX, float worldY, float& height);
};

