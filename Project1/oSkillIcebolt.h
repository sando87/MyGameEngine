#pragma once
#include "jGameObject.h"

class jShaderDefault;
class cParticleSystem;

class oSkillIcebolt :
	public jGameObject
{
protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	jShaderDefault* mShader;
	cParticleSystem *mFrostTrace;
	cParticleSystem *mSmokeTrace;
	cParticleSystem *mSparklingEffect;

	void CreateFrostTrace();
	void CreateSmokeTrace();
	void CreateSparklingEffect();
	void EditVertices();


	Property_GetSetter(Vector3, StartPos, Vector3())
	Property_GetSetter(Vector3, DestPos, Vector3())
};

