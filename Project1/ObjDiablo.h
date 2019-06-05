#pragma once

#include "jGameObject.h"
class jModel;
class jShaderDiablo;
class jShaderColor;
class jShaderSkinned;
class jTexture;

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
	jShaderSkinned * mShader;
	jTexture * mTexture;
};

