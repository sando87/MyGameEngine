#include "ObjTerrainMgr.h"
#include "ObjCamera.h"
#include "ObjTerrain.h"
#include "jHeightMap.h"

#define TERRAIN_SIZE 240
#define TERRAIN_STEP 5

ObjTerrainMgr::ObjTerrainMgr()
{
}


ObjTerrainMgr::~ObjTerrainMgr()
{
	for (auto item : mBlockResourcesAll)
		delete item.second;

	mBlockResourcesAll.clear();
}

void ObjTerrainMgr::OnStart()
{
	//ObjTerrain* obj = new ObjTerrain();
	//vector<string>* names = new vector<string>();
	//names->push_back("MyObject_14");
	//obj->Load(names);
	//obj->AddToMgr();

	mBlockSize = TERRAIN_SIZE;
	LoadTerrainGridMetaInfo();
}
void ObjTerrainMgr::OnUpdate()
{
	LoadingBlocks();
	
	if (mCachedBlocks.size() > 30)
		ClearFarBlocks(15);
}


void ObjTerrainMgr::LoadingBlocks()
{
	jRect rect = GetCamera().GetGroundRect();
	int idxStartX = (int)(rect.Left() / mBlockSize);
	int idxEndX =  (int)(rect.Right() / mBlockSize);
	int idxStartY = (int)(rect.Bottom() / mBlockSize);
	int idxEndY =  (int)(rect.Top() / mBlockSize);
	for (int idxY = idxStartY; idxY <= idxEndY; ++idxY)
	{
		for (int idxX = idxStartX; idxX <= idxEndX; ++idxX)
		{
			u64 key = GRID_HASH(idxX, idxY);
			if (mBlockResourcesAll.find(key) == mBlockResourcesAll.end())
				continue;

			if (mCachedBlocks.find(key) == mCachedBlocks.end())
			{
				TerrainBlock block;
				block.heightMap = new jHeightMap(TERRAIN_SIZE, TERRAIN_SIZE, TERRAIN_STEP);
				LoadBlock(idxX, idxY, block);
				mCachedBlocks[key] = block;
			}
		}
	}
}

void ObjTerrainMgr::LoadBlock(int idxX, int idxY, TerrainBlock& block)
{
	u64 key = GRID_HASH(idxX, idxY);
	vector<string>* names = mBlockResourcesAll[key];
	for (string name : *names)
	{
		ObjTerrain* obj = new ObjTerrain();
		obj->Load(name, TERRAIN_SIZE, TERRAIN_STEP, block.heightMap);
		obj->AddToMgr();
		block.terrains.push_back(obj);
	}
	float minZ = block.heightMap->MinHeight();
	float maxZ = block.heightMap->MaxHeight();
	block.rect = jRect3D(Vector3(idxX*TERRAIN_SIZE, idxY*TERRAIN_SIZE, minZ), Vector3(TERRAIN_SIZE, TERRAIN_SIZE, maxZ - minZ));
	return;
}
void ObjTerrainMgr::ClearFarBlocks(int clearCount)
{
	jRect camRect = GetCamera().GetGroundRect();
	vector<pair<double,u64>> distances;
	for (auto item : mCachedBlocks)
	{
		jRect rect = item.second.rect.TopBottom();
		Vector2 pt = rect.Center();
		u64 key = GRID_HASH(rect.Left(), rect.Bottom());
		double dist = camRect.Center().distance(pt);
		distances.push_back(pair<double, u64>(dist, key));
	}
	
	sort(distances.begin(), distances.end(), [&](pair<double, u64> left, pair<double, u64> right) {
		return left.first > right.first;
	});

	for (int i = 0; i < clearCount; ++i)
	{
		u64 key = distances[i].second;
		TerrainBlock& block = mCachedBlocks[key];
		jRect rect = block.rect.TopBottom();
		if (camRect.Overlapped(rect))
			break;

		for(ObjTerrain* terrain : block.terrains)
			terrain->DeleteFromMgr();

		mCachedBlocks.erase(key);
	}
}

void ObjTerrainMgr::LoadTerrainGridMetaInfo()
{
	string path = "D:\\export\\D3\\";
	jUtils::ForEachFiles2(nullptr, (path+"*.*").c_str(), [&](void *obj, string name) {
		string fullname = path + name + "\\" + name + ".txt";
		strings lines = jUtils::LoadLines(fullname);
		if (lines->size() != 19)
			return true;

		string firstLine = lines->front();
		strings coordinates = jUtils::Split2(firstLine, " ");
		int x = atoi(coordinates[0].c_str());
		int y = atoi(coordinates[1].c_str());
		mTerrainCenter.x += x;
		mTerrainCenter.y += y;
		mTerrainCenter.z += 1;
		u64 key = CoordinateToKey(x, y);
		if (mBlockResourcesAll.find(key) == mBlockResourcesAll.end())
		{
			mBlockResourcesAll[key] = new vector<string>;
			mBlockResourcesAll[key]->push_back(name);
		}
		else
		{
			mBlockResourcesAll[key]->push_back(name);
		}
		
		return true;
	});

	mTerrainCenter.x /= mTerrainCenter.z;
	mTerrainCenter.y /= mTerrainCenter.z;
	mTerrainCenter.x = 0;
	mTerrainCenter.y = 0;
}
bool ObjTerrainMgr::GetHeight(float worldX, float worldY, float& height)
{
	u64 key = CoordinateToKey(worldX, worldY);
	if (mCachedBlocks.find(key) == mCachedBlocks.end())
		return false;

	TerrainBlock& block = mCachedBlocks[key];
	jRect rect = block.rect.TopBottom();
	height = block.heightMap->GetHeightOfPos(worldX - rect.Left(), worldY - rect.Bottom());
	return true;
}
Vector3 ObjTerrainMgr::CalcGroundPos(Vector3 pos, Vector3 dir)
{
	dir.normalize();
	Vector3 currentPos = pos;
	while (currentPos.z > 0)
	{
		currentPos += (dir * TERRAIN_STEP);

		float height = 0;
		if (!GetHeight(currentPos.x, currentPos.y, height))
			continue;

		if (currentPos.z < height)
			return currentPos;
	}
	return Vector3();
}

//ObjTerrain* ObjTerrainMgr::GetTerrain(float worldX, float worldY)
//{
//	u64 key = CoordinateToKey(worldX, worldY);
//	if (mCachedBlocks.find(key) == mCachedBlocks.end())
//		return nullptr;
//
//	return mCachedBlocks[key];
//}
