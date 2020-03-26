#include "ObjBomb.h"
#include "jTransform.h"
#include "jTime.h"
#include "jGameObjectMgr.h"
#include "ObjParticle.h"
#include "jParticle.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderDefault.h"
#include "jTinyDB.h"
#include "jHealthPoint.h"
#include "ObjTerrainMgr.h"
#include "ObjEnemy.h"

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

	class BombParticle : public Particle
	{
	public:
		virtual void OnUpdate()
		{
			Force(Vector3(0, 0, -150));
			Particle::OnUpdate();
			Death = false;
		}
	};

	Vector2 dir = mDestPos - GetTransform().getPos();
	dir.normalize();

	mParticleBomb = new jParticle();
	mParticleBomb->OnCreateParticle = []() { return new BombParticle(); };
	mParticleBomb->SetPosition(GetTransform().getPos());
	mParticleBomb->SetForce(10000);
	mParticleBomb->SetDirection(Vector3(dir.x, dir.y, 3));
	mParticleBomb->SetDegree(0);
	mParticleBomb->SetCount(1);
	mParticleBomb->SetMassRate(3);
	mParticleBomb->SetDuration(1);
	mParticleBomb->SetBurstCount(1);
	mParticleBomb->SetBurstIntervalSec(0);
	AddComponent(mParticleBomb);

	CreateFlameParticle();
	//CreateSmokeParticle();
}

void ObjBomb::OnStart()
{
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();;
}

void ObjBomb::OnUpdate()
{
	auto iter = mParticleBomb->GetParticles().begin();
	if (iter != mParticleBomb->GetParticles().end())
	{
		Particle* part = *iter;
		GetTransform().moveTo(part->Pos);
		mParticleFlame->SetDirection(part->Vel * -1);
	
		Vector3 dir = mParticleBomb->GetDirection();
		dir.z = (jUtils::Random() % 8) * 0.1;
		dir.normalize();
		GetTransform().rotateAxis(dir, 180 * jTime::Delta());
	}
	
	Vector3 pos = GetTransform().getPos();
	float height = 0;
	mTerrain->GetHeight(pos.x, pos.y, height);
	if(pos.z < height)
	{
		CreateExploreParticle();
		mParticleFlame->SetBurstCount(0);
		AttackEnemies();
		Destroy();
	}
	
}

void ObjBomb::CreateFlameParticle()
{

	class FlameParticle : public Particle
	{
	public:
		FlameParticle(jGameObject * parent)
		{
			texIdx = 0;
			size = 3;
			Pos = parent->GetTransform().getPos();
		}
		virtual void OnUpdate()
		{
			Force(Vector3(0, 0, 20));
			Particle::OnUpdate();
			texIdx = 4 - (int)(AccTime / 0.1);
			Death = (texIdx < 0) ? true : false;
		}
	};

	mParticleFlame = new jParticle();
	mParticleFlame->OnCreateParticle = [this]() { return new FlameParticle(this); };
	mParticleFlame->SetForce(300);
	mParticleFlame->SetDirection(mParticleBomb->GetDirection() * -1);
	mParticleFlame->SetDegree(20);
	mParticleFlame->SetRandomRate(0.5);
	mParticleFlame->SetCount(1);
	mParticleFlame->SetBurstCount(50);
	mParticleFlame->SetBurstIntervalSec(0.07);

	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetParticle(mParticleFlame);
	objParticle->SetImageFullname("./res/img/" + mResImgFlame);
	objParticle->SetImageGridCount(Vector2n(8, 1));
	objParticle->SetImageStepUV(Vector2f(0.125f, 1.0f));
	GetEngine().AddGameObject(objParticle);
}

void ObjBomb::CreateExploreParticle()
{
	class MyParticle : public Particle
	{
	public:
		jLinear colorGraph;
		MyParticle()
		{
			texIdx = jUtils::Random() % 4;
			size = 3;
			colorGraph.Init(-1, 1);
		}
		virtual void OnUpdate()
		{
			Particle::OnUpdate();
			float scale = colorGraph.CalcY(AccTime);
			color = Vector4f(scale + 0.3, scale + 0.3, scale + 0.3, scale * 4);
		}
	};

	mParticleExplore = new jParticle();
	mParticleExplore->OnCreateParticle = []() { return new MyParticle(); };
	mParticleExplore->SetPosition(GetTransform().getPos());
	mParticleExplore->SetForce(1000);
	mParticleExplore->SetDegree(90);
	mParticleExplore->SetCount(40);
	mParticleExplore->SetCoeffDrag(0.5);
	mParticleExplore->SetDuration(1);
	mParticleExplore->SetGravity(Vector3(0, 0, 9.8));
	mParticleExplore->SetBurstCount(1);
	mParticleExplore->SetBurstIntervalSec(0);

	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetParticle(mParticleExplore);
	objParticle->SetImageFullname("./res/img/" + mResImgExplore);
	objParticle->SetImageGridCount(Vector2n(2, 2));
	objParticle->SetImageStepUV(Vector2f(0.5f, 0.5f));
	GetEngine().AddGameObject(objParticle);
}

void ObjBomb::CreateSmokeParticle()
{
	class SmokeParticle : public Particle
	{
	public:
		Vector3 mParentPos;
		SmokeParticle(jGameObject * parent)
		{
			texIdx = 2;
			size = 1;
			refDiscard = 1;
			mass = 0;
			color = Vector4f(0.3f, 0.3f, 0.3f, 1.0f);
			mParentPos = parent->GetTransform().getPos();
		}
		virtual void OnUpdate()
		{
			size += 1 / 30.0f;
			if (size > 2)
			{
				color.w -= 1 / 120.0f;
				refDiscard -= 1 / 60.0f;
			}

			Pos = mParentPos;
			if (color.w <= 0)
				Death = true;
		}
	};

	mParticleSmoke = new jParticle();
	mParticleSmoke->OnCreateParticle = [this]() { return new SmokeParticle(this); };
	mParticleSmoke->SetForce(10);
	mParticleSmoke->SetDegree(10);
	mParticleSmoke->SetCount(1);
	mParticleSmoke->SetStartDelay(4);
	mParticleSmoke->SetBurstCount(10);
	mParticleSmoke->SetBurstIntervalSec(0.3);

	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetParticle(mParticleSmoke);
	objParticle->SetImageFullname("./res/img/" + mResImgSmoke);
	objParticle->SetImageGridCount(Vector2n(4, 1));
	objParticle->SetImageStepUV(Vector2f(0.25f, 1.0f));
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
