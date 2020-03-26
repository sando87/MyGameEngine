#pragma once
#include "jShader.h"

class ObjCamera;
class jUISystem;

class jShaderUI :
	public jShader
{
public:
	jShaderUI(jUISystem* form);
	virtual ~jShaderUI();

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);

	void DrawRect(VertexFormatPTC verticies[4], void* textureView);

protected:
	void LoadLayout();
	void LoadMesh();
	void LoadTextureWhite();
	
	void ConfigRender();

	ObjCamera* mCameraUI;
	jUISystem* mForm;
};

