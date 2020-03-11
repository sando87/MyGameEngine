#pragma once
#include "jGameObject.h"

class jMesh;
class jParticle;
class jShaderHPBar;
struct ShaderBufferHPBars;

class ObjHealthBars :
	public jGameObject
{
public:
	void ShowHPBar(jGameObject* obj);

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	jMesh* CreateHpMesh(int count);

	jParticle* mParticle;
	jShaderHPBar* mShader;
	ShaderBufferHPBars* mParamsBars;
};

