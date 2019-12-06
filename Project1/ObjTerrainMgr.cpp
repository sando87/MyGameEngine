#include "ObjTerrainMgr.h"
#include "jUtils.h"
#include "jLog.h"
#include "jTypeDef.h"
#include "ObjCamera.h"
#include "ObjTerrain.h"
#include <algorithm>

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
	int idxStartX = rect.Left() / mBlockSize;
	int idxEndX = rect.Right() / mBlockSize;
	int idxStartY = rect.Bottom() / mBlockSize;
	int idxEndY = rect.Top() / mBlockSize;
	for (int idxY = idxStartY; idxY <= idxEndY; ++idxY)
	{
		for (int idxX = idxStartX; idxX <= idxEndX; ++idxX)
		{
			u64 key = GRID_HASH(idxX, idxY);
			if (mBlockResourcesAll.find(key) == mBlockResourcesAll.end())
				continue;

			if (mCachedBlocks.find(key) == mCachedBlocks.end())
			{
				ObjTerrain* obj = LoadBlock(idxX, idxY);
				mCachedBlocks[key] = obj;
			}
		}
	}
}

ObjTerrain* ObjTerrainMgr::LoadBlock(int idxX, int idxY)
{
	u64 key = GRID_HASH(idxX, idxY);
	vector<string>* names = mBlockResourcesAll[key];
	ObjTerrain* obj = new ObjTerrain();
	obj->Load(names, TERRAIN_SIZE, TERRAIN_STEP);
	obj->AddToMgr();
	return obj;
}
void ObjTerrainMgr::ClearFarBlocks(int clearCount)
{
	jRect camRect = GetCamera().GetGroundRect();
	vector<pair<double,u64>> distances;
	for (auto item : mCachedBlocks)
	{
		ObjTerrain* terrain = item.second;
		jRect rect = terrain->GetRect().TopBottom();
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
		ObjTerrain* obj = mCachedBlocks[key];
		jRect rect = obj->GetRect().TopBottom();
		if (camRect.Overlapped(rect))
			break;

		obj->DeleteFromMgr();
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
}
float ObjTerrainMgr::GetHeight(float worldX, float worldY)
{
	ObjTerrain* obj = GetTerrain(worldX, worldY);
	if (obj == nullptr)
		return 0;

	return obj->GetHeight(worldX, worldY);
}
Vector3 ObjTerrainMgr::CalcGroundPos(Vector3 pos, Vector3 dir)
{
	dir.normalize();
	Vector3 currentPos = pos;
	while (true)
	{
		ObjTerrain* obj = GetTerrain(currentPos.x, currentPos.y);
		if (obj == nullptr)
			break;

		float height = obj->GetHeight(currentPos.x, currentPos.y);
		if (currentPos.z < height)
			return currentPos;

		currentPos += (dir * TERRAIN_STEP);
	}
	return Vector3();
}
ObjTerrain* ObjTerrainMgr::GetTerrain(float worldX, float worldY)
{
	u64 key = CoordinateToKey(worldX, worldY);
	if (mCachedBlocks.find(key) == mCachedBlocks.end())
		return nullptr;

	return mCachedBlocks[key];
}
