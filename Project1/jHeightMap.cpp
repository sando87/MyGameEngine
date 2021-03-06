#include "jHeightMap.h"
#include "jMesh.h"

jHeightMap::~jHeightMap()
{
}

bool jHeightMap::UpdateHeightMap(jMesh * _mesh, Vector3 _base)
{
	if (mHeightMap.empty())
		mHeightMap.resize(mCntWidth * mCntHeight);

	vector<VertexFormat>& verticies = _mesh->GetVerticies();
	int cnt = verticies.size();
	for (int idx = 0; idx < cnt; ++idx)
	{
		VertexFormat& vert = verticies[idx];
		float x = vert.position.x;
		float y = vert.position.y;
		if (x < 0 || mWidth <= x || y < 0 || mHeight <= y)
			continue;

		int gridIdx = IdxOfNearPt(x, y);
		mHeightMap[gridIdx] = max(mHeightMap[gridIdx], vert.position.z + (float)_base.z);
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

