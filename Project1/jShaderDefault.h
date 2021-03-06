#pragma once
#include "jShader.h"

class jImage;
class jMesh;

class jShaderDefault :
	public jShader
{
public:
	jShaderDefault();
	virtual ~jShaderDefault();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

protected:
	void LoadLayout();
	void LoadMesh();

	jMesh* mMesh;
	jImage* mImage;

	Property_GetSetter(string, VSFullname, "")
	Property_GetSetter(string, PSFullname, "")
};