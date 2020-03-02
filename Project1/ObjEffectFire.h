#pragma once
#include "jGameObject.h"

class jParticle;
class jShaderEffectTrace;
struct ShaderBufferBillboards;

class ObjEffectFire :
	public jGameObject
{
public:
	ObjEffectFire();
	~ObjEffectFire();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	void LoadMesh(int count);

	jParticle * mPaticles;
	jShaderEffectTrace * mShader;
	ShaderBufferBillboards* mParamsBillboards;
	
};

