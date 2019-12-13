#pragma once
#include "jGameObject.h"
#include "junks.h"

class jHeightMap;

class ObjTerrain :
	public jGameObject
{
public:
	ObjTerrain();
	~ObjTerrain();
private:
	//void OnStart();
	//void OnUpdate();
	//void OnDraw();
	
public:
	void Load(string name, float size, float step, jHeightMap* heightmap);
};

