#include "ObjParticle.h"
#include "cParticleSystem.h"
#include "jMesh.h"
#include "jShaderEffectTrace.h"
#include "jImage.h"
#include "ObjCamera.h"

ObjParticle::ObjParticle()
{
	mParticleSystem = new cParticleSystem();;
}

ObjParticle::~ObjParticle()
{
	delete mParticleSystem;
}

void ObjParticle::OnLoad()
{
	AddComponent(mParticleSystem);

	jMesh* mesh = CreateParticleMesh(mParticleSystem->GetMaxCount());
	AddComponent(mesh);

	AddComponent(new jImage(mImageFullname));

	mShader = new jShaderEffectTrace();
	mShader->GetParamBasic().uvInfo = Vector4f(mImageGridCount.x, mImageGridCount.y, mImageStepUV.x, mImageStepUV.y);
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	mShader->SetRenderOrder(RenderOrder_Terrain_Env_Alpha);
	mShader->SetAlphaBlackOn(mImageBlendBlack);
	AddComponent(mShader);
}

void ObjParticle::OnStart()
{
	ObjCamera* cam = GetEngine().FindGameObject<ObjCamera>();
	jTransform trans;
	trans.lookat(Vector3(), cam->GetTransform().getView(), Vector3(0, 0, 1));
	mShader->GetParamBillboard().billboardMat = trans.getLocalMatrix().transpose();
}

void ObjParticle::OnUpdate()
{
	ShaderBufferBillboards& param = mShader->GetParamBillboard();
	memset(param.boards, 0x00, sizeof(param.boards));

	int idx = 0;
	for (Particle* part : mParticleSystem->GetParticles())
	{
		param.boards[idx].transform.x = part->Pos.x;
		param.boards[idx].transform.y = part->Pos.y;
		param.boards[idx].transform.z = part->Pos.z;
		param.boards[idx].size = part->size;
		param.boards[idx].texIndex = part->texIdx;
		param.boards[idx].refDiscard = part->refDiscard;
		param.boards[idx].rotate = part->rotate;
		param.boards[idx].reserve = part->reserve;
		param.boards[idx].color = part->color;
		idx++;
	}
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

