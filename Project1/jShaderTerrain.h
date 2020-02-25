#pragma once
#include "jShader.h"

class jMesh;
class jImage;

class jShaderTerrain :
	public jShader
{
public:
	jShaderTerrain();
	~jShaderTerrain();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

protected:
	void LoadLayout();
	void LoadMesh();

	jMesh* mMesh;
	vector<jImage*> mImages;
};

