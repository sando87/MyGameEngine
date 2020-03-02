#include "ObjEffectFire.h"
#include "jMesh.h"
#include "jImage.h"
#include "jTransform.h"
#include "ObjCamera.h"
#include "jShaderEffectTrace.h"
#include "jTime.h"
#include "jParticle.h"
#include "jLinear.h"

class MyParticle : public Particle
{
public:
	int imgIndex;
	jLinear color;
	jLinear alpha;
	MyParticle()
	{
		imgIndex = jUtils::Random() % 4;
		color.Init(-1, 1);
	}
	virtual void OnUpdate()
	{
		Particle::OnUpdate();
	}
};

ObjEffectFire::ObjEffectFire()
{
}

ObjEffectFire::~ObjEffectFire()
{
}
void ObjEffectFire::OnLoad()
{

	mPaticles = new jParticle();
	mPaticles->OnCreateParticle = []() { return new MyParticle(); };
	mPaticles->SetForce(1000);
	mPaticles->SetDegree(90);
	mPaticles->SetCount(40);
	mPaticles->SetCoeffDrag(0.5);
	mPaticles->SetDuration(1);
	mPaticles->SetGravity(Vector3(0, 0, 9.8));
	mPaticles->SetStart(false);
	mPaticles->SetBurstCount(1);
	mPaticles->SetBurstIntervalSec(0.02);
	AddComponent(mPaticles);

	LoadMesh(40);

	AddComponent(new jImage("./res/img/explore.tga"));
	//AddComponent(new jImage("./res/img/103_0000028EBB2AF120_t.tga"));

	mShader = new jShaderEffectTrace();
	mShader->GetParamBasic().uvInfo = Vector4f(2.0f, 2.0f, 0.5f, 0.5f);
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	AddComponent(mShader);

	mParamsBillboards = &mShader->GetParamBillboard();

}
void ObjEffectFire::OnStart()
{
	ObjCamera* cam = GetEngine().FindGameObject<ObjCamera>();
	jTransform trans;
	trans.lookat(Vector3(), cam->GetTransform().getView(), Vector3(0, 0, 1));
	mParamsBillboards->billboardMat = trans.getLocalMatrix().transpose();
}
void ObjEffectFire::OnUpdate()
{
	static double tt = 0;
	tt += jTime::Delta();
	if (tt > 3)
		mPaticles->SetStart(true);

	int idx = 0;
	auto particles = mPaticles->GetParticles();
	for (Particle* ptc : particles)
	{
		MyParticle* particle = (MyParticle*)ptc;
		mParamsBillboards->boards[idx].transform.x = particle->Pos.x;
		mParamsBillboards->boards[idx].transform.y = particle->Pos.y;
		mParamsBillboards->boards[idx].transform.z = particle->Pos.z;
		mParamsBillboards->boards[idx].size = 3;
		mParamsBillboards->boards[idx].texIndex = particle->imgIndex;
		float color = particle->color.CalcY(particle->Time);
		mParamsBillboards->boards[idx].color = Vector4f(color + 0.3, color + 0.3, color + 0.3, color * 4);
		idx++;
		if (idx >= 40)
			break;
	}

}
void ObjEffectFire::LoadMesh(int count)
{
	jMesh* mesh = new jMesh();
	Vector2 size(1, 1);
	Vector2 stepUV(0.5, 0.5);
	vector<VertexFormat> verticies;
	vector<u32> indicies;
	for (int i = 0; i < count; ++i)
	{
		VertexFormat vert[4];
		vert[0].position = Vector3(-size.x, 0, -size.y);
		vert[1].position = Vector3(size.x, 0, -size.y);
		vert[2].position = Vector3(-size.x, 0, size.y);
		vert[3].position = Vector3(size.x, 0, size.y);
		vert[0].texel = Vector2f(0, stepUV.y);
		vert[1].texel = Vector2f(stepUV.x, stepUV.y);
		vert[2].texel = Vector2f(0, 0);
		vert[3].texel = Vector2f(stepUV.x, 0);
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

	mesh->LoadVerticies(verticies, indicies, "MeshEffectTracer");
	AddComponent(mesh);
}