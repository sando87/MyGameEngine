#pragma once
#include "jGameObject.h"
#include "jParabolic2.h"

class cParticleSystem;

class ObjBomb :
	public jGameObject
{
public:
	ObjBomb();
	~ObjBomb();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	void CreateFlameParticle();
	void CreateExploreParticle();
	void CreateSmokeParticle();

	void AttackEnemies();

	cParticleSystem *mParticleBomb;
	cParticleSystem *mParticleFlame;
	cParticleSystem *mParticleExplore;
	cParticleSystem *mParticleSmoke;

	string mResImgBomb;
	string mResImgFlame;
	string mResImgExplore;
	string mResImgSmoke;

	Property_GetSetter(Vector3, DestPos, Vector3())
	Property_GetSetter(jGameObject*, Owner, nullptr)
};

