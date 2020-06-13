#include "ObjBomb.h"
#include "jTransform.h"
#include "jTime.h"
#include "jGameObjectMgr.h"
#include "ObjParticle.h"
#include "cParticleSystem.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderDefault.h"
#include "jTinyDB.h"
#include "jHealthPoint.h"
#include "ObjEnemy.h"
#include "jTerrainCollider.h"

ObjBomb::ObjBomb()
{
}

ObjBomb::~ObjBomb()
{
}

void ObjBomb::OnLoad()
{
	DBSkill skillDB;
	skillDB.Load(1);
	strings imgs = jUtils::Split2(skillDB.resImg, "&");
	_warnif(imgs->size() != 4);

	mResImgBomb = imgs[0];
	mResImgFlame = imgs[1];
	mResImgExplore = imgs[2];
	mResImgSmoke = imgs[3];

	AddComponent(new jHealthPoint(skillDB.spec));

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + skillDB.resMesh));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + mResImgBomb));
	jShaderDefault* shader = new jShaderDefault();
	shader->SetRenderOrder(RenderOrder_Terrain_Env_Alpha);
	AddComponent(shader);

	jTerrainCollider* terCol = new jTerrainCollider();
	terCol->SetOnTerrain(false);
	terCol->EventUnderTerrain = [&](double height) {
		CreateExploreParticle();
		mParticleFlame->SetBurstCount(0);
		AttackEnemies();
		Destroy();
	};
	AddComponent(terCol);

	Vector2 dir = mDestPos - GetTransform().getPos();
	dir.normalize();

	mParticleBomb = new cParticleSystem();
	mParticleBomb->SetPosition(GetTransform().getPos());
	mParticleBomb->SetForce(10000);
	mParticleBomb->SetDirection(Vector3(dir.x, dir.y, 3));
	mParticleBomb->SetDegree(0);
	mParticleBomb->SetCount(1);
	mParticleBomb->SetMassRate(3);
	mParticleBomb->SetDuration(1);
	mParticleBomb->SetBurstCount(1);
	mParticleBomb->SetBurstIntervalSec(0);
	mParticleBomb->EventUpdateParticle = [this](Particle& part) {
		part.forces.push_back(Vector3(0, 0, -150));
		GetTransform().moveTo(part.Pos);
		mParticleFlame->SetDirection(part.Vel * -1);
		Vector3 dir = mParticleBomb->GetDirection();
		dir.z = (jUtils::Random() % 8) * 0.1;
		dir.normalize();
		GetTransform().rotateAxis(dir, 180 * jTime::Delta());
		part.Death = false;
	};
	AddComponent(mParticleBomb);

	CreateFlameParticle();
	//CreateSmokeParticle();

}

void ObjBomb::OnStart()
{
}

void ObjBomb::OnUpdate()
{
}

void ObjBomb::CreateFlameParticle()
{
	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetImageFullname("./res/img/" + mResImgFlame);
	objParticle->SetImageGridCount(Vector2n(8, 1));
	objParticle->SetImageStepUV(Vector2f(0.125f, 1.0f));

	mParticleFlame = objParticle->GetParticleSystem();
	mParticleFlame->SetForce(300);
	mParticleFlame->SetDirection(mParticleBomb->GetDirection() * -1);
	mParticleFlame->SetDegree(20);
	mParticleFlame->SetRandomRate(0.5);
	mParticleFlame->SetCount(1);
	mParticleFlame->SetBurstCount(50);
	mParticleFlame->SetBurstIntervalSec(0.07);

	mParticleFlame->EventStartParticle = [this](Particle& particle) {
		particle.texIdx = 0;
		particle.size = 3;
		particle.Pos = GetTransform().getPos();
	};
	mParticleFlame->EventUpdateParticle = [&](Particle& particle) {
		particle.forces.push_back(Vector3(0, 0, 20));
		particle.texIdx = 4 - (int)(particle.AccTime / 0.1);
		particle.Death = (particle.texIdx < 0) ? true : false;
	};

	GetEngine().AddGameObject(objParticle);
}

void ObjBomb::CreateExploreParticle()
{
	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetImageFullname("./res/img/" + mResImgExplore);
	objParticle->SetImageGridCount(Vector2n(2, 2));
	objParticle->SetImageStepUV(Vector2f(0.5f, 0.5f));

	mParticleExplore = objParticle->GetParticleSystem();
	mParticleExplore->SetPosition(GetTransform().getPos());
	mParticleExplore->SetForce(1000);
	mParticleExplore->SetDegree(90);
	mParticleExplore->SetCount(40);
	mParticleExplore->SetCoeffDrag(0.5);
	mParticleExplore->SetDuration(1);
	mParticleExplore->SetGravity(Vector3(0, 0, 9.8));
	mParticleExplore->SetBurstCount(1);
	mParticleExplore->SetBurstIntervalSec(0);
	mParticleExplore->EventStartParticle = [this](Particle& part) {
		part.texIdx = jUtils::Random() % 4;
		part.size = 3;
		part.reserve = -1.0;
	};
	mParticleExplore->EventUpdateParticle = [this](Particle& part) {
		double slope = 2.0;
		part.reserve += slope * jTime::Delta(); //expect -1 ~ 1
		float scale = part.reserve;
		part.color = Vector4f(scale + 0.3, scale + 0.3, scale + 0.3, scale * 4);
	};

	GetEngine().AddGameObject(objParticle);
}

void ObjBomb::CreateSmokeParticle()
{
	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetImageFullname("./res/img/" + mResImgSmoke);
	objParticle->SetImageGridCount(Vector2n(4, 1));
	objParticle->SetImageStepUV(Vector2f(0.25f, 1.0f));

	mParticleSmoke = objParticle->GetParticleSystem();
	mParticleSmoke->SetForce(10);
	mParticleSmoke->SetDegree(10);
	mParticleSmoke->SetCount(1);
	mParticleSmoke->SetStartDelay(4);
	mParticleSmoke->SetBurstCount(10);
	mParticleSmoke->SetBurstIntervalSec(0.3);
	mParticleSmoke->EventStartParticle = [this](Particle& particle) {
		particle.texIdx = 2;
		particle.size = 1;
		particle.refDiscard = 1;
		particle.mass = 0;
		particle.color = Vector4f(0.3f, 0.3f, 0.3f, 1.0f);
	};
	mParticleSmoke->EventUpdateParticle = [this](Particle& particle) {
		particle.size += 1 / 30.0f;
		if (particle.size > 2)
		{
			particle.color.w -= 1 / 120.0f;
			particle.refDiscard -= 1 / 60.0f;
		}

		particle.Pos = GetTransform().getPos();;
		if (particle.color.w <= 0)
			particle.Death = true;
	};

	GetEngine().AddGameObject(objParticle);
}

void ObjBomb::AttackEnemies()
{
	Vector3 pos = GetTransform().getPos();
	jHealthPoint* owner = mOwner->FindComponent<jHealthPoint>();
	jHealthPoint* hp = FindComponent<jHealthPoint>();
	auto enemies = GetEngine().FindGameObjects<ObjEnemy>();
	for (ObjEnemy* enemy : *enemies)
	{
		Vector3 targetPos = enemy->GetTransform().getPos();
		if (targetPos.distance(pos) < 5)
			owner->Attack(enemy->FindComponent<jHealthPoint>(), hp);
	}
}
