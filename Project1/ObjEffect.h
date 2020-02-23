#pragma once
#include "jGameObject.h"

class jShaderSprite;

class ObjEffect :
	public jGameObject
{
public:
	ObjEffect();
	virtual ~ObjEffect();

	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	jShaderSprite * mShader;
	int idx = 0;
};

