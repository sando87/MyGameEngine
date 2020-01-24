#pragma once
#include "jShader.h"

class jMesh;

class jShaderColor :
	public jShader
{
public:
	jShaderColor();
	virtual ~jShaderColor();
	
	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

protected:
	void LoadLayout();
	void LoadMesh();

	jMesh* mMesh;
};

