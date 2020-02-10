#pragma once
#include "jGameObject.h"

class ObjCreateHeightmap : public jGameObject
{
public:
	friend class jEventHeightMap;
	ObjCreateHeightmap();
	virtual ~ObjCreateHeightmap();

protected:
	virtual void OnStart();

	void LoadBlocksInfo();
	void SetCamera(int x, int y, int _min, int _max);
	void CaptureAndSaveHeightMap(int idx);
	bool FindMinMaxHeight(string fullname, Vector2& result);
	vector<Vector4n> mBlocks;
};

