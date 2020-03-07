#include "ObjParticle.h"
#include "jParticle.h"
#include "jMesh.h"
#include "jShaderEffectTrace.h"
#include "jImage.h"
#include "ObjCamera.h"

ObjParticle::ObjParticle()
{
}


ObjParticle::~ObjParticle()
{
}

void ObjParticle::OnLoad()
{
	if (mParticle == nullptr)
	{
		mParticle = new jParticle();
	}
	AddComponent(mParticle);

	jMesh* mesh = CreateParticleMesh(mParticle->GetReserve());
	AddComponent(mesh);

	AddComponent(new jImage(mImageFullname));

	mShader = new jShaderEffectTrace();
	mShader->GetParamBasic().uvInfo = Vector4f(mImageGridCount.x, mImageGridCount.y, mImageStepUV.x, mImageStepUV.y);
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	AddComponent(mShader);

	mParamsBillboards = &mShader->GetParamBillboard();
}

void ObjParticle::OnStart()
{
	ObjCamera* cam = GetEngine().FindGameObject<ObjCamera>();
	jTransform trans;
	trans.lookat(Vector3(), cam->GetTransform().getView(), Vector3(0, 0, 1));
	mParamsBillboards->billboardMat = trans.getLocalMatrix().transpose();
}

void ObjParticle::OnUpdate()
{
	list<Particle*>::iterator iter = mParticle->GetParticles().begin();
	list<Particle*>::iterator end = mParticle->GetParticles().end();
	int count = mParticle->GetReserve();
	for (int i = 0; i < count; ++i)
	{
		if (iter != end)
		{
			Particle* particle = *iter;
			mParamsBillboards->boards[i].transform.x = particle->Pos.x;
			mParamsBillboards->boards[i].transform.y = particle->Pos.y;
			mParamsBillboards->boards[i].transform.z = particle->Pos.z;
			mParamsBillboards->boards[i].size = particle->size;
			mParamsBillboards->boards[i].texIndex = particle->texIdx;
			mParamsBillboards->boards[i].refDiscard = particle->refDiscard;
			mParamsBillboards->boards[i].color = particle->color;
			++iter;
		}
		else
		{
			mParamsBillboards->boards[i] = Billboard();
			mParamsBillboards->boards[i].size = 0;
		}
	}

	if (mParticle->IsFinished())
		Destroy();
}
jMesh* ObjParticle::CreateParticleMesh(int count)
{
	jMesh* mesh = new jMesh();
	vector<VertexFormat> verticies;
	vector<u32> indicies;
	for (int i = 0; i < count; ++i)
	{
		VertexFormat vert[4];
		vert[0].position = Vector3(-0.5, 0, -0.5);
		vert[1].position = Vector3(0.5, 0, -0.5);
		vert[2].position = Vector3(-0.5, 0, 0.5);
		vert[3].position = Vector3(0.5, 0, 0.5);
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

	mesh->LoadVerticies(verticies, indicies, "MeshEffectTracer" + jUtils::ToString(count));
	return mesh;
}