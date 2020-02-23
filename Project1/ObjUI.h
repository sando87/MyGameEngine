#pragma once
#include "jGameObject.h"
#include "jShaderHeader.h"
#include <jUISystem.h>

class jUISystem;
class jShaderUIEngine;
class jInputEvent;

class ObjUI :
	public jGameObject
{
public:
	ObjUI();
	virtual ~ObjUI();

	virtual void OnLoad();
	virtual void OnUpdate();

protected:
	jUISystem * mUIEngine;
	jShaderUIEngine * mShader;
	jInputEvent * mEvent;

	void ConvertDrawParam(DrawingParams& params, VertexFormatPTC vert[4]);
};

