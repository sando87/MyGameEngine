#pragma once

#include "junks.h"

#include "tinyxml2.h"
using namespace tinyxml2;

struct ObjectInfo
{
	vector<string>   Info;
	vector<Vector3f> Pos;
	vector<Vector2f> UV;
	vector<Vector3f> Normal;
	vector<Vector4n> BoneIndexs;
	vector<Vector4f> Weights;
	vector<Vector3n> VertexIdx;
};

typedef shared_ptr_array<tinyxml2::XMLElement*> xmlElements;

class jLoader
{
public:

	jLoader() {}
	~jLoader() { Release(); }

	vector<ObjectInfo> mObjects;

	void LoadObjFile(string _filename);
	void LoadDaeFile(string _filename);

	void Release() {}

private:
	xmlElements GetObjects(tinyxml2::XMLDocument& _doc);
	void ParseMeshes(tinyxml2::XMLElement* _ele, ObjectInfo& info);
	void ParseWeights(tinyxml2::XMLElement* _ele, ObjectInfo& info);
};

