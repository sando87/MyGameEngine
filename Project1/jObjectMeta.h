#pragma once
#include "junks.h"
#include "jRect3D.h"

class jObjectMeta
{
public:
	jObjectMeta();
	~jObjectMeta();

	string filename;
	string objname;
	string type_shader;
	bool alpha;
	bool depth;
	Vector3 worldPos;
	string animName;
	vector<Vector4f> texels;
	vector<string> imgFileNames;
	bool Load(string name);

	string GetObjFullName() { return PATH_RESOURCES + string("mesh/") + objname; }
	string GetImgFullName(int idx) { return PATH_RESOURCES + string("img/") + imgFileNames[idx]; }
	string GetAnimFullName() { return PATH_RESOURCES + string("anim/") + animName; }
};

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
	float map[];
};

class jZMapLoader
{
public:
	jZMapLoader() { Data = nullptr; }
	~jZMapLoader() {
		if (Data) {
			free(Data);
			Data = nullptr;
		}
	}

	string Filename;
	jRect3D Rect3D;
	ZMapHeader *Data;
	bool Load(string fullname);
	static bool Save(string fullname, jRect3D rt, int step, u32* pixels, int imgWidth);
};