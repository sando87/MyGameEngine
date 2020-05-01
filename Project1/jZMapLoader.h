#pragma once
#include "junks.h"
#include "jRect3D.h"

struct ZMapHeader
{
	int step;
	int minZ;
	int maxZ;
};

class jZMapLoader
{
public:
	jZMapLoader() {}
	~jZMapLoader() {}

	string Filename;
	jRect3D Rect3D;
	ZMapHeader Header;
	vector<float> Heights;
	vector<bool> Accessable;
	bool LoadHeights(string name);
	bool LoadAccessables(string name);
	bool GetHeight(float worldX, float worldY, float& outHeight);
	static bool Save(string fullname, jRect3D rt, int step, u32* pixels, int imgWidth);
};

