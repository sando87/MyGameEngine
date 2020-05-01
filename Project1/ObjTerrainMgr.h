#pragma once
#include "jGameObject.h"
#include "jRectangle2D.h"
#include "jGridContainer.h"

class ObjCamera;

struct TerrainBlock
{
	vector<jGameObject*> terrains;
	jRectangle2D rect;
	//jZMapLoader* zMap;
};

class ObjTerrainMgr :
	public jGameObject
{
public:
	friend class cHeightMap;
	ObjTerrainMgr();
	virtual ~ObjTerrainMgr();

private:
	virtual void OnStart();
	virtual void OnUpdate();

	
	unordered_map<uint64_t, vector<string>*> mBlockResourcesAll;
	unordered_map<uint64_t, TerrainBlock> mCachedBlocks;
	jGridContainer<Vector3> mHeights;
	ObjCamera* mCamera;
	int mBlockSize;

	void LoadTerrainGridMetaInfo();
	void LoadingBlocks(jRectangle2D rect);
	void ClearFarBlocks(int clearCount);
	void LoadTerrainsInBlock(int idxX, int idxY, TerrainBlock& block);
	void LoadHeights(vector<Vector3>& worldPositions);
	u64 CoordinateToKey(float x, float y)
	{
		int nx = (int)(x / mBlockSize);
		int ny = (int)(y / mBlockSize);
		u64 key = ToU64(nx, ny);
		return key;
	}

public:
	bool RayCastTerrain(Vector3 pos, Vector3 dir, Vector3& outPoint);
	bool FindObstacle(Vector2 start, Vector2 end, Vector2& obstaclePos, double step);
	bool GetHeight(Vector2 worldPos, float& outHeight);
	bool GetHeightSimple(Vector2 worldPos, float& outHeight);
};

