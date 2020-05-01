#include "ObjTerrainMgr.h"
#include "ObjCamera.h"
#include "ObjTerrain.h"
#include "jHeightMap.h"
#include "jMesh.h"
#include "jZMapLoader.h"
#include "jGameObjectMgr.h"

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
	mCamera = (ObjCamera *)GetEngine().FindGameObject("ObjCamera");
	mHeights.SetStep(TERRAIN_STEP);
	LoadTerrainGridMetaInfo();
}
void ObjTerrainMgr::OnUpdate()
{
	jRectangle2D rect = mCamera->GetGroundRect();
	LoadingBlocks(rect);
	
	if (mCachedBlocks.size() > 100)
		ClearFarBlocks(50);
}


void ObjTerrainMgr::LoadingBlocks(jRectangle2D rect)
{
	int idxStartX = (int)(rect.GetMin().x / mBlockSize);
	int idxEndX =  (int)(rect.GetMax().x / mBlockSize);
	int idxStartY = (int)(rect.GetMin().y / mBlockSize);
	int idxEndY =  (int)(rect.GetMax().y / mBlockSize);
	for (int idxY = idxStartY; idxY <= idxEndY; ++idxY)
	{
		for (int idxX = idxStartX; idxX <= idxEndX; ++idxX)
		{
			u64 key = ToU64(idxX, idxY);
			if (mBlockResourcesAll.find(key) == mBlockResourcesAll.end())
				continue;

			if (mCachedBlocks.find(key) == mCachedBlocks.end())
			{
				string name = to_string(idxX * TERRAIN_SIZE) + "_" + to_string(idxY * TERRAIN_SIZE);

				TerrainBlock block;
				block.rect.SetPosSize(Vector2(idxX * TERRAIN_SIZE, idxY * TERRAIN_SIZE), Vector2(TERRAIN_SIZE, TERRAIN_SIZE));
				//block.zMap = new jZMapHeights();
				//block.zMap->LoadHeights(PATH_RESOURCES + string("zmap/") + name);
				//block.zMap->LoadAccessables(PATH_RESOURCES + string("zmap/") + name);

				LoadTerrainsInBlock(idxX, idxY, block);

				mCachedBlocks[key] = block;
			}
		}
	}
}

void ObjTerrainMgr::LoadTerrainsInBlock(int idxX, int idxY, TerrainBlock& block)
{
	string foldername = to_string(idxX * TERRAIN_SIZE) + "_" + to_string(idxY * TERRAIN_SIZE);
	u64 key = ToU64(idxX, idxY);
	vector<string>* names = mBlockResourcesAll[key];
	for (string name : *names)
	{
		ObjTerrain* obj = new ObjTerrain();
		obj->Load(foldername + "/" + name);
		GetEngine().AddGameObject(obj);
		block.terrains.push_back(obj);
	}
	return;
}
void ObjTerrainMgr::LoadHeights(vector<Vector3>& worldPositions)
{
	unordered_map<u64, pair<int, double>> tmp;
	for (Vector3 pos : worldPositions)
	{
		u64 key = mHeights.ToKey(pos);
		if (tmp.find(key) == tmp.end())
		{
			tmp[key] = make_pair(1, pos.z);
		}
		else
		{
			tmp[key].first++;
			tmp[key].second += pos.z;
		}
	}

	for (auto elem : tmp)
	{
		u64 key = elem.first;
		int n = elem.second.first;
		double heightSum = elem.second.second;
		double heightAvg = heightSum / n;
		Vector2 gridPos = mHeights.ToPosition(key);
		if (mHeights.IsContains(key))
		{
			double preHeight = mHeights[key].z;
			mHeights[key] = Vector3(gridPos.x, gridPos.y, (heightAvg + preHeight) / 2);
		}
		else
		{
			mHeights[key] = Vector3(gridPos.x, gridPos.y, heightAvg);
		}
	}
}
void ObjTerrainMgr::ClearFarBlocks(int clearCount)
{
	jRectangle2D camRect = mCamera->GetGroundRect();
	vector<pair<double,u64>> distances;
	for (auto item : mCachedBlocks)
	{
		jRectangle2D rect = item.second.rect;
		Vector2 pt = rect.GetCenter();
		Vector2 min = rect.GetMin();
		u64 key = ToU64(min.x, min.y);
		double dist = camRect.GetCenter().distance(pt);
		distances.push_back(pair<double, u64>(dist, key));
	}
	
	sort(distances.begin(), distances.end(), [&](pair<double, u64> left, pair<double, u64> right) {
		return left.first > right.first;
	});

	for (int i = 0; i < clearCount; ++i)
	{
		u64 key = distances[i].second;
		TerrainBlock& block = mCachedBlocks[key];
		jRectangle2D rect = block.rect;
		if (camRect.IsOverlapped(rect))
			break;

		for(jGameObject* terrain : block.terrains)
			terrain->Destroy();

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
bool ObjTerrainMgr::RayCastTerrain(Vector3 pos, Vector3 dir, Vector3& outPoint)
{
	double gridStep = mHeights.GetStep();
	Vector3 startPos = pos;
	Vector3 retPos = pos;
	double curStep = 25.0;
	int n = 1;
	while (curStep > 0.1)
	{
		Vector3 curPos = startPos + dir * curStep * n;
		float height = 0;
		bool isValid = curStep >= gridStep ? GetHeightSimple(curPos, height) : GetHeight(curPos, height);
		if (!isValid)
			return false;
	
		if (curPos.z < height)
		{
			startPos = curPos - dir * curStep;
			retPos = (startPos + curPos) / 2;
			curStep /= 5.0;
			n = 1;
		}
		else
			n++;
	}
	outPoint = retPos;
	return true;

	//double step = mHeights.GetStep();
	//dir.normalize();
	//Vector3 currentPos = pos;
	//while (true)
	//{
	//	currentPos += (dir);
	//
	//	float height = -1000;
	//	if (!GetHeight(currentPos, height))
	//		break;
	//
	//	if (currentPos.z < height)
	//	{
	//		outPoint = currentPos;
	//		outPoint.z = height;
	//		return true;
	//	}
	//}
	//return false;
}

bool ObjTerrainMgr::FindObstacle(Vector2 start, Vector2 end, Vector2 & obstaclePos, double step)
{
	Vector2 dir = end - start;
	dir.normalize();
	dir *= step;
	Vector2 curPos = start;
	float height = 0;
	while (curPos.distance(end) > step)
	{
		bool ret = GetHeight(curPos, height);
		if (!ret)
		{
			obstaclePos = curPos;
			return true;
		}
		curPos += dir;
	}
	obstaclePos = end;
	return false;
}

bool ObjTerrainMgr::GetHeight(Vector2 worldPos, float & outHeight)
{
	u64 key = CoordinateToKey(worldPos.x, worldPos.y);
	//_exceptif(mCachedBlocks.find(key) == mCachedBlocks.end(), return false);
	//_exceptif(!mHeights.IsContains(worldPos), return false);

	//Interpolate 4 edge heights of rectangle.
	double step = mHeights.GetStep();
	Vector3 lb = mHeights[Vector2(worldPos.x - step / 2, worldPos.y - step / 2)];
	Vector3 lt = mHeights[Vector2(lb.x, lb.y + step)];
	Vector3 rb = mHeights[Vector2(lb.x + step, lb.y)];
	Vector3 rt = mHeights[Vector2(lb.x + step, lb.y + step)];
	double areaTotal = step * step;
	double rateLB = (rt.x - worldPos.x) * (rt.y - worldPos.y) / areaTotal;
	double rateLT = (rb.x - worldPos.x) * (worldPos.y - rb.y) / areaTotal;
	double rateRB = (worldPos.x - lt.x) * (lt.y - worldPos.y) / areaTotal;
	double rateRT = (worldPos.x - lb.x) * (worldPos.y - lb.y) / areaTotal;
	outHeight = lb.z * rateLB + lt.z * rateLT + rb.z * rateRB + rt.z * rateRT;
	return true;
}

bool ObjTerrainMgr::GetHeightSimple(Vector2 worldPos, float & outHeight)
{
	u64 key = CoordinateToKey(worldPos.x, worldPos.y);
	//_exceptif(mCachedBlocks.find(key) == mCachedBlocks.end(), return false);
	//_exceptif(!mHeights.IsContains(worldPos), return false);
	Vector3 pos = mHeights[worldPos];
	outHeight = pos.z;
	return true;
}

