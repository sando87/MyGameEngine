#pragma once
#include "jGameObject.h"

class jMesh;
class cParticleSystem;
class jShaderHPBar;
class jHealthPoint;

struct HpDrawInfo
{
	jGameObject* obj;
	jHealthPoint* hp;
	double lifeTime;
	double accTime;
};

class ObjHealthBars :
	public jGameObject
{
public:
	void ShowHPBar(jHealthPoint* hp);
	void DeleteHPBar(jHealthPoint* hp);

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	jMesh* CreateHpMesh(int count);

	cParticleSystem* mParticle;
	jShaderHPBar* mShader;
	unordered_map<jHealthPoint*, HpDrawInfo> mHPBars;
};

