#include "ObjHealthBars.h"
#include "jParticle.h"
#include "jMesh.h"
#include "jShaderHPBar.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jTransform.h"
#include "jHealthPoint.h"

class HPParticle : public Particle
{
public:
	jTransform* partTransform;
	jHealthPoint* partHP;
	HPParticle(jGameObject* obj)
	{
		partTransform = obj->FindComponent<jTransform>();
		partHP = obj->FindComponent<jHealthPoint>();
	}
	virtual void OnUpdate()
	{
		Pos = partTransform->getPos();
		refDiscard = partHP->CurrentStatus.life / partHP->BasicStatus.life; //hp Rate
		Death = (LifeTime < AccTime) ? true : false;
	}
};

void ObjHealthBars::ShowHPBar(jGameObject * obj)
{
	HPParticle* particle = new HPParticle(obj);
	mParticle->Burst(particle);
}

void ObjHealthBars::OnLoad()
{
	mParticle = new jParticle();
	mParticle->SetCount(1);
	mParticle->SetMassRate(0);
	mParticle->SetDuration(5);
	mParticle->SetBurstCount(0); //infinite mode
	AddComponent(mParticle);

	jMesh* mesh = CreateHpMesh(mParticle->GetReserve());
	AddComponent(mesh);

	AddComponent(new jImage("./res/img/hpbar.png"));

	mShader = new jShaderHPBar();
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	AddComponent(mShader);

	mParamsBars = &mShader->GetParamBars();
}

void ObjHealthBars::OnUpdate()
{
	list<Particle*>::iterator iter = mParticle->GetParticles().begin();
	list<Particle*>::iterator end = mParticle->GetParticles().end();
	int count = mParticle->GetReserve();
	for (int i = 0; i < count; ++i)
	{
		if (iter != end)
		{
			Particle* particle = *iter;
			mParamsBars->bars[i].transform.x = particle->Pos.x;
			mParamsBars->bars[i].transform.y = particle->Pos.y;
			mParamsBars->bars[i].transform.z = particle->Pos.z;
			mParamsBars->bars[i].size.x = particle->size;
			mParamsBars->bars[i].size.y = particle->size;
			mParamsBars->bars[i].reserve = 0;
			mParamsBars->bars[i].hpRate = particle->refDiscard;
			mParamsBars->bars[i].color = particle->color;
			++iter;
		}
		else
		{
			mParamsBars->bars[i] = HPInfo();
			mParamsBars->bars[i].size = Vector2f();
		}
	}
}

jMesh * ObjHealthBars::CreateHpMesh(int count)
{
	jMesh* mesh = new jMesh();
	vector<VertexFormat> verticies;
	vector<u32> indicies;
	for (int i = 0; i < count; ++i)
	{
		VertexFormat vert[4];
		vert[0].position = Vector3(-0.05, -0.01, 0);
		vert[1].position = Vector3(0.05, -0.01, 0);
		vert[2].position = Vector3(-0.05, 0.01, 0);
		vert[3].position = Vector3(0.05, 0.01, 0);
		vert[0].texel = Vector2f(0, 1);
		vert[1].texel = Vector2f(1, 1);
		vert[2].texel = Vector2f(0, 0);
		vert[3].texel = Vector2f(1, 0);
		vert[0].boneIndexs[0] = i;
		vert[1].boneIndexs[0] = i;
		vert[2].boneIndexs[0] = i;
		vert[3].boneIndexs[0] = i;

		verticies.push_back(vert[0]);
		verticies.push_back(vert[1]);
		verticies.push_back(vert[2]);
		verticies.push_back(vert[3]);

		indicies.push_back(i * 4 + 0);
		indicies.push_back(i * 4 + 1);
		indicies.push_back(i * 4 + 2);
		indicies.push_back(i * 4 + 2);
		indicies.push_back(i * 4 + 1);
		indicies.push_back(i * 4 + 3);
	}

	mesh->LoadVerticies(verticies, indicies, "HealthBars" + jUtils::ToString(count));
	return mesh;
}
