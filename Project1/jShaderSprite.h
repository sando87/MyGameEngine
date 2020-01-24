#pragma once
#include "jShader.h"

class jMesh;
class jImage;

class jShaderSprite : 
	public jShader
{
public:
	jShaderSprite();
	virtual ~jShaderSprite();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

protected:
	void LoadLayout();
	void LoadMesh();
	void LoadTexture();

	jMesh* mMesh;
	jImage* mImage;
};

