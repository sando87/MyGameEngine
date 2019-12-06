#pragma once
#include "jGameObject.h"
#include "jParserD3.h"
#include "jRect3D.h"
#include "jTypeDef.h"

class jModel;
class jShaderTerrain;
class jTexture;
class jHeightMap;

class TerrainSubObject
{
public:
	std::string name;
	jModel* model;
	vector<jTexture*> textures;
	vector<Vector4f> texels;
	Vector3 position;
	bool Load(std::string name, int size);
};

class ObjTerrain :
	public jGameObject
{
public:
	ObjTerrain();
	~ObjTerrain();
private:
	void OnStart();
	void OnUpdate();
	void OnDraw();
	
	jModel* mModel;
	jTexture * mTexture[6];

	jHeightMap* mHeightMap;
	jShaderTerrain * mShader;
	jRect3D mRect;

public:
	jRect3D GetRect() { return mRect; }
	vector<TerrainSubObject*> mSubObjects;
	void Load(vector<string>* names, float size, float step);
	float GetHeight(float x, float y);

	int mFileIndex;
	jParserD3 mRenderIfno;
	Vector4f mTexels[12];
};

