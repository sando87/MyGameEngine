#pragma once
#include "jGameObject.h"

class jMesh;
class cParticleSystem;
class jShaderEffectTrace;
struct ShaderBufferBillboards;

class ObjParticle :
	public jGameObject
{
public:
	ObjParticle();
	virtual ~ObjParticle();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	jMesh* CreateParticleMesh(int count);

	jShaderEffectTrace* mShader;

	Property_Getter(cParticleSystem *, ParticleSystem, nullptr)
	Property_Setter(string, ImageFullname, "")
	Property_Setter(Vector2n, ImageGridCount, Vector2n())
	Property_Setter(Vector2f, ImageStepUV, Vector2f())
	Property_Setter(bool, ImageBlendBlack, false)
};

