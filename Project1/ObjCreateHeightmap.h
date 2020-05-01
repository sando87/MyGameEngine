#pragma once
#include "jGameObject.h"

class ObjCreateHeightmap : public jGameObject
{
public:
	ObjCreateHeightmap();
	virtual ~ObjCreateHeightmap();

protected:
	virtual void OnStart();

	vector<Vector4n> mBlocks;
	int mCurrentGridIndex;

	void LoadBlocksInfo();
	void SetCamera(int x, int y, int _min, int _max);
	void CaptureAndSaveHeightMap(int idx);
	bool FindMinMaxHeight(string fullname, Vector2& result);
	void OnKeyDown(char ch);
	
};

