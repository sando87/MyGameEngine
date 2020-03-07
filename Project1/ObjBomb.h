#pragma once
#include "jGameObject.h"
#include "jParabolic2.h"

class jParticle;

class ObjBomb :
	public jGameObject
{
public:
	ObjBomb();
	~ObjBomb();

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	void CreateFlameParticle();
	void CreateExploreParticle();
	void CreateSmokeParticle();

	jParticle *mParticleBomb;
	jParticle *mParticleFlame;
	jParticle *mParticleExplore;
	jParticle *mParticleSmoke;
};

