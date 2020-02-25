#pragma once
#include "jShader.h"

class jImage;
class jMesh;

class jShaderSkin :
	public jShader
{
public:
	jShaderSkin();
	virtual ~jShaderSkin();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

protected:
	void LoadLayout();
	void LoadMesh();

	jMesh* mMesh;
	jImage* mImage;

};

