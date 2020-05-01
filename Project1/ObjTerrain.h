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

	void Load(string filename);
protected:
	void OnStart();
	//void OnUpdate();

	string mMetaFullname = "";
	jGameObject* mChild = nullptr;

	bool SetTypeTxt(string value);
	string GetTypeTxt();
	void CreateChildCollisionBox();
	void Show(bool isShow);
	void OnKeyDown(char key);
	
};

