#include "ObjTerrainMgr.h"
#include "ObjCamera.h"
#include "ObjTerrain.h"
#include "jHeightMap.h"
#include "jMesh.h"
#include "jZMapLoader.h"
#include "jGameObjectMgr.h"

#define TERRAIN_SIZE 240
#define TERRAIN_STEP 5

INITIALIZER(terrain)
{
	jGameObjectMgr::GetInst().AddGameObject(new ObjTerrainMgr());
}

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
	
	if (mCachedBlocks.size() > 100)
		ClearFarBlocks(50);
}


void ObjTerrainMgr::LoadingBlocks()
{
	jRect rect = mEngine->GetCamera().GetGroundRect();
	int idxStartX = (int)(rect.Left() / mBlockSize);
	int idxEndX =  (int)(rect.Right() / mBlockSize);
	int idxStartY = (int)(rect.Bottom() / mBlockSize);
	int idxEndY =  (int)(rect.Top() / mBlockSize);
	for (int idxY = idxStartY; idxY <= idxEndY; ++idxY)
	{
		for (int idxX = idxStartX; idxX <= idxEndX; ++idxX)
		{
			u64 key = ToU64(idxX, idxY);
			if (mBlockResourcesAll.find(key) == mBlockResourcesAll.end())
				continue;

			if (mCachedBlocks.find(key) == mCachedBlocks.end())
			{
				string filename = to_string(idxX * TERRAIN_SIZE) + "_" + to_string(idxY * TERRAIN_SIZE) + ".zmap";

				TerrainBlock block;
				block.zMap = new jZMapLoader();
				block.zMap->Load(PATH_RESOURCES + string("zmap/") + filename);

				LoadBlock(idxX, idxY, block);

				mCachedBlocks[key] = block;
			}
		}
	}
}

void ObjTerrainMgr::LoadBlock(int idxX, int idxY, TerrainBlock& block)
{
	string foldername = to_string(idxX * TERRAIN_SIZE) + "_" + to_string(idxY * TERRAIN_SIZE);
	u64 key = ToU64(idxX, idxY);
	vector<string>* names = mBlockResourcesAll[key];
	for (string name : *names)
	{
		ObjTerrain* obj = new ObjTerrain();
		obj->Load(foldername + "/" + name);
		mEngine->AddGameObject(obj);
		block.terrains.push_back(obj);
	}
	return;
}
void ObjTerrainMgr::ClearFarBlocks(int clearCount)
{
	jRect camRect = mEngine->GetCamera().GetGroundRect();
	vector<pair<double,u64>> distances;
	for (auto item : mCachedBlocks)
	{
		jRect rect = item.second.zMap->Rect3D.TopBottom();
		Vector2 pt = rect.Center();
		u64 key = ToU64(rect.Left(), rect.Bottom());
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
		jRect rect = block.zMap->Rect3D.TopBottom();
		if (camRect.Overlapped(rect))
			break;

		for(jGameObject* terrain : block.terrains)
			terrain->SetRemove(true);

		mCachedBlocks.erase(key);
	}
}

void ObjTerrainMgr::LoadTerrainGridMetaInfo()
{
	string path = PATH_RESOURCES + string("meta/");

	//폴더별로 루프
	jUtils::ForEachFiles2(nullptr, (path+"*").c_str(), [&](void *obj, string filename) {
		strings coordinates = jUtils::Split2(filename, "_");
		if (coordinates->size() != 2)
			return true;

		int x = atoi(coordinates[0].c_str());
		int y = atoi(coordinates[1].c_str());
		u64 key = CoordinateToKey(x, y);
		if (mBlockResourcesAll.find(key) == mBlockResourcesAll.end())
			mBlockResourcesAll[key] = new vector<string>;
		vector<string>* list = mBlockResourcesAll[key];

		//각폴더안의 txt파일별로 루프
		jUtils::ForEachFiles2(nullptr, (path + filename + "/*.*").c_str(), [&](void *obj, string filename) {
			list->push_back(filename);
			return true;
		});
		
		return true;
	});

}
bool ObjTerrainMgr::GetHeight(float worldX, float worldY, float& height)
{
	u64 key = CoordinateToKey(worldX, worldY);
	if (mCachedBlocks.find(key) == mCachedBlocks.end())
		return false;

	TerrainBlock& block = mCachedBlocks[key];
	bool ret = block.zMap->GetHeight(worldX, worldY, height);
	return ret;
}
bool ObjTerrainMgr::RayCastTerrain(Vector3 pos, Vector3 dir, Vector2& outPoint)
{
	dir.normalize();
	Vector3 currentPos = pos;
	bool validPos = false; 
	while (currentPos.z > 0)
	{
		currentPos += (dir * TERRAIN_STEP);

		float height = 0;
		if (!GetHeight(currentPos.x, currentPos.y, height))
		{
			if (validPos)
			{
				//클릭지점의 terrain값이 비정상적일 경우 마지막 유효했던 지점 출력(예외 루틴)
				outPoint = currentPos - (dir * TERRAIN_STEP);
				validPos = false;
			}
			continue;
		}

		validPos = true;
		if (currentPos.z < height)
		{
			//클릭지점의 terrain값이 정상적일 경우 유효한 지점 출력(정상적인 루틴)
			outPoint = currentPos;
			return true;
		}
	}
	return false;
}

bool ObjTerrainMgr::Reachable(Vector2 start, Vector2 end, Vector2 & lastPoint, double step)
{
	Vector2 dir = end - start;
	dir.normalize();
	dir *= step;
	Vector2 curPos = start;
	float height = 0;
	while (curPos.distance(end) > step)
	{
		bool ret = GetHeight(curPos.x, curPos.y, height);
		if (!ret)
		{
			lastPoint = curPos;
			return false;
		}
		curPos += dir;
	}
	lastPoint = end;
	return true;
}

//ObjTerrain* ObjTerrainMgr::GetTerrain(float worldX, float worldY)
//{
//	u64 key = CoordinateToKey(worldX, worldY);
//	if (mCachedBlocks.find(key) == mCachedBlocks.end())
//		return nullptr;
//
//	return mCachedBlocks[key];
//}
