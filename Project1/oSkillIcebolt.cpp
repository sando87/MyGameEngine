#include "oSkillIcebolt.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderDefault.h"
#include "jTransform.h"
#include "jGameObjectMgr.h"
#include "ObjParticle.h"
#include "cParticleSystem.h"
#include "ObjCamera.h"
#include "jTime.h"

void oSkillIcebolt::OnLoad()
{
	GetTransform().lookat(mStartPos, mDestPos, Vector3(0, 0, 1));
	GetTransform().Zoom(Vector3(0.2, 0.2, 0.2));

	jMesh* mesh = new jMesh(string(PATH_RESOURCES) + "mesh/00000203250B5798_101.dump");
	AddComponent(mesh);

	jImage* img = new jImage(string(PATH_RESOURCES) + "img/101_00000203913861E0_t.dump");
	AddComponent(img);

	mShader = new jShaderDefault();
	mShader->SetVSFullname("./shaders/iceBolt.vs");
	mShader->SetPSFullname("./shaders/iceBolt.ps");
	AddComponent(mShader);

	mShader->GetParamBasic().matSpecular = Vector4f(1, 1, 1, 1);
	mShader->GetParamBasic().lightDirection = Vector4f(-1, 1, -1, 0).normalize();

	CreateFrostTrace();
	CreateSmokeTrace();
	CreateSparklingEffect();

	//GetEngine().StartCoRoutine("oSkillIceboltStartMove", 3000, [this](CorMember& mem, bool first) {
	//	return CorCmd::Stop;
	//});

}
void oSkillIcebolt::OnUpdate()
{
	Vector3 camPos = GetEngine().FindGameObject<ObjCamera>()->GetTransform().getPos();
	mShader->GetParamBasic().lightReserve = Vector4f(camPos.x, camPos.y, camPos.z, 0);

	Vector3 dir = mDestPos - mStartPos;
	dir.normalize();
	Vector3 newPos = GetTransform().getPos() + dir * 25 * jTime::Delta();
	GetTransform().moveTo(newPos);

	GetTransform().rotateAxis(GetTransform().getView(), 10);

}

void oSkillIcebolt::CreateFrostTrace()
{
	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetImageFullname("./res/img/100_00000203C72E20E0_t.tga");
	objParticle->SetImageGridCount(Vector2n(4, 4));
	objParticle->SetImageStepUV(Vector2f(0.25f, 0.25f));

	mFrostTrace = objParticle->GetParticleSystem();
	//mFrostTrace->SetForce(300);
	mFrostTrace->SetGravity(Vector3(0, 0, 0));
	//mFrostTrace->SetDirection(Vector3(0, 1, 0));
	//mFrostTrace->SetDegree(20);
	//mFrostTrace->SetRandomRate(0.1);
	mFrostTrace->SetCount(1);
	mFrostTrace->SetBurstCount(50);
	mFrostTrace->SetBurstIntervalSec(0.1);

	mFrostTrace->EventStartParticle = [this](Particle& particle) {
		particle.texIdx = jUtils::Random() % 16;
		particle.Pos = GetTransform().getPos();
		particle.size = 1.0f;
		particle.refDiscard = 0;
	};
	mFrostTrace->EventUpdateParticle = [&](Particle& particle) {
		particle.size += 0.2f;
		particle.refDiscard += 0.04f;
		if (particle.refDiscard > 1)
			particle.Death = true;
		else
			particle.Death = false;
		//particle.size += 0.1f;
		//if (particle.color.w > 0)
		//	particle.color.w -= 0.002f;
	};

	GetEngine().AddGameObject(objParticle);
}

void oSkillIcebolt::CreateSmokeTrace()
{
	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetImageFullname("./res/img/iceBoltSmoke.tga");
	objParticle->SetImageGridCount(Vector2n(2, 2));
	objParticle->SetImageStepUV(Vector2f(0.5f, 0.5f));

	mSmokeTrace = objParticle->GetParticleSystem();
	//mSmokeTrace->SetForce(300);
	mSmokeTrace->SetGravity(Vector3(0, 0, 0));
	//mSmokeTrace->SetDirection(Vector3(0, -1, 0));
	//mSmokeTrace->SetDegree(20);
	//mSmokeTrace->SetRandomRate(0.1);
	mSmokeTrace->SetCount(1);
	mSmokeTrace->SetBurstCount(50);
	mSmokeTrace->SetBurstIntervalSec(0.1);

	mSmokeTrace->EventStartParticle = [this](Particle& particle) {
		particle.texIdx = jUtils::Random() % 4;
		particle.size = 4;
		particle.Pos = GetTransform().getPos();
		particle.color = Vector4f(1.5f, 1.5f, 1.5f, 1.0f);
	};
	mSmokeTrace->EventUpdateParticle = [&](Particle& particle) {
		particle.size += 0.2f;
		particle.color.w -= 0.04f;
		if (particle.color.w <= 0)
			particle.Death = true;
		else
			particle.Death = false;
	};

	GetEngine().AddGameObject(objParticle);
}

void oSkillIcebolt::CreateSparklingEffect()
{
	ObjParticle* objParticle = new ObjParticle();
	objParticle->SetImageFullname("./res/img/116_000002038ABA2760_t.tga");
	objParticle->SetImageGridCount(Vector2n(1, 1));
	objParticle->SetImageStepUV(Vector2f(1.0f, 1.0f));
	objParticle->SetImageBlendBlack(true);

	mSparklingEffect = objParticle->GetParticleSystem();
	mSparklingEffect->SetCount(1);
	mSparklingEffect->SetBurstCount(30);
	mSparklingEffect->SetBurstIntervalSec(0.3);

	mSparklingEffect->EventStartParticle = [&](Particle& particle) {
		particle.Pos = GetTransform().getPos() + (GetTransform().getView() * 0.3);
		particle.size = 5.0f + (jUtils::Random() % 5);
		particle.rotate = (jUtils::Random() % 300) * 0.01f;
		particle.color = Vector4f(1, 1, 1, 1);
		particle.LifeTime = 0.3f;
		mSparklingEffect->SetBurstIntervalSec(0.1f + ((jUtils::Random() % 100) * 0.001f));
	};
	mSparklingEffect->EventUpdateParticle = [&](Particle& particle) {
		particle.Pos = GetTransform().getPos() + (GetTransform().getView() * 0.3);
		//particle.rotate += 0.1f;
		particle.color *= 0.9f;
		if (particle.AccTime > 1)
			particle.Death = true;
	};

	GetEngine().AddGameObject(objParticle);
}

void oSkillIcebolt::EditVertices()
{
	Matrix4 rotMat;
	rotMat.identity();
	rotMat.rotateZ(-90);
	
	jMesh* mesh = FindComponent<jMesh>();
	vector<VertexFormat>& verts = mesh->GetVerticies();
	for (VertexFormat& vert : verts)
	{
		//vert.texel.x = (vert.position.y / 13.0) + 0.5;
		//vert.texel.y = (vert.position.z / 13.0) + 0.5;
		//Vector3 pos = vert.position;
		//Vector3 rotPos = pos * rotMat;
		//vert.position = rotPos;

		Vector3 nor = vert.normal;
		Vector3 rotNor = nor * rotMat;
		vert.normal = rotNor;
	}

	jUtils::SaveToFile(mesh->GetFullname(), (char*)&verts[0], sizeof(VertexFormat)*verts.size());
}
