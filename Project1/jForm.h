#pragma once
#include "jComponent.h"

class jUISystem;
class jShaderUI;
struct DrawingParams;
struct VertexFormatPTC;

class jForm :
	public jComponent
{
public:
	jForm(jUISystem* form);
	virtual ~jForm();

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	jUISystem * mForm;
	jShaderUI * mShader;

	void ConvertDrawParam(DrawingParams& params, VertexFormatPTC vert[4]);

	Property_GetSetter(string, JsonFilename, "")
};

