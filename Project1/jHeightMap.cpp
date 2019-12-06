#include "jHeightMap.h"
#include "jModel.h"
#include "jGlobalStruct.h"

jHeightMap::~jHeightMap()
{
	if (mHeightMap != nullptr)
		delete[] mHeightMap;
}

bool jHeightMap::UpdateHeightMap(jModel * _mesh, float _off)
{
	if (mHeightMap == nullptr)
	{
		mHeightMap = new float[mCntWidth * mCntHeight];
		memset(mHeightMap, 0, sizeof(float) * mCntWidth * mCntHeight);
	}

	vector<VertexType_Texture>& verticies = _mesh->GetVerticies();
	int cnt = verticies.size();
	for (int idx = 0; idx < cnt; ++idx)
	{
		VertexType_Texture& vert = verticies[idx];
		float x = vert.p.x;
		float y = vert.p.y;
		int gridIdx = IdxOfNearPt(x, y);
		mHeightMap[gridIdx] = max(mHeightMap[gridIdx], vert.p.z + _off);
		mMinZ = min(mHeightMap[gridIdx], mMinZ);
		mMaxZ = max(mHeightMap[gridIdx], mMaxZ);
	}

	return true;
}

int jHeightMap::IdxOfNearPt(float x, float y)
{
	int idxX = (int)(x / (float)mStep);
	int idxY = (int)(y / (float)mStep);
	return ToIndex(idxX, idxY);
}

void jHeightMap::SetHeight(float x, float y, float z)
{
	int idx = IdxOfNearPt(x, y);
	mHeightMap[idx] = z;
}

float jHeightMap::GetHeightOfIdx(int idxX, int idxY)
{
	int idx = ToIndex(idxX, idxY);
	return mHeightMap[idx];
}

Vector2f jHeightMap::GetPos(int idxX, int idxY)
{
	return Vector2f(idxX * mStep, idxY * mStep);
}

float jHeightMap::GetHeightOfPos(float x, float y)
{
	int idx = IdxOfNearPt(x, y);
	return mHeightMap[idx];
}

