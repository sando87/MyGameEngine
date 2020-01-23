#pragma once
#include "junks.h"
#include "jRect3D.h"

struct ZMapHeader
{
	char ext[8];
	int size;
	int step;
	int minX;
	int minY;
	int minZ;
	int maxX;
	int maxY;
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
	vector<float> Map;
	bool Load(string fullname);
	bool GetHeight(float worldX, float worldY, float& outHeight);
	static bool Save(string fullname, jRect3D rt, int step, u32* pixels, int imgWidth);
};
