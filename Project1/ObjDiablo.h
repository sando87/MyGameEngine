#pragma once

#include "jGameObject.h"
class jModel;
class jShaderDiablo;

class ObjDiablo : public jGameObject
{
public:
	ObjDiablo();
	~ObjDiablo();

private:
	void OnStart();
	void OnUpdate();
	void OnDraw();

	jModel* mModel;
	jShaderDiablo * mShader;
};

