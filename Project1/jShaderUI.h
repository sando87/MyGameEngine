#pragma once
#include "jShader.h"

class ObjCamera;

class jShaderUI :
	public jShader
{
public:
	jShaderUI();
	virtual ~jShaderUI();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam) = 0;

	void DrawRect(VertexFormatPTC verticies[4], void* textureView);

protected:
	void LoadLayout();
	void LoadMesh();
	
	void ConfigRender();

	ObjCamera* mCameraUI;
};

