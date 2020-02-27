#include "ObjEffect.h"
#include "jShaderEffectTrace.h"
#include "jMesh.h"
#include "jImage.h"
#include "jTransform.h"
#include "ObjCamera.h"

ObjEffect::ObjEffect()
{
}


ObjEffect::~ObjEffect()
{
}

void ObjEffect::Burst(Vector3 pos, int idxU, int idxV)
{
	if (mParamsBillboards == nullptr)
		return;

	mParamsBillboards->boards[mBillboardIndex].worldMat = mBillboardMat;
	mParamsBillboards->boards[mBillboardIndex].worldMat[3] = pos.x;
	mParamsBillboards->boards[mBillboardIndex].worldMat[7] = pos.y;
	mParamsBillboards->boards[mBillboardIndex].worldMat[11] = pos.z;
	mParamsBillboards->boards[mBillboardIndex].texelIndex.x = idxU;
	mParamsBillboards->boards[mBillboardIndex].texelIndex.y = idxV;
	mParamsBillboards->boards[mBillboardIndex].refDiscard = 1.0f;

	mBillboardIndex = (mBillboardIndex + 1) % mBillboardCount;
}

void ObjEffect::OnLoad()
{
	LoadMesh();

	jImage* imgae = new jImage(mImgFullname);
	AddComponent(imgae);

	mShader = new jShaderEffectTrace();
	mShader->GetParamBasic().spriteStep = mStepUV;
	mParamsBillboards = &mShader->GetParamBillboard();
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	AddComponent(mShader);

	mBillboardIndex = 0;
	mBillboardCount = sizeof(mParamsBillboards->boards) / sizeof(mParamsBillboards->boards[0]);
}

void ObjEffect::OnStart()
{
	ObjCamera* cam = GetEngine().FindGameObject<ObjCamera>();
	jTransform trans;
	trans.lookat(Vector3(), cam->GetTransform().getView(), Vector3(0, 0, 1));
	mBillboardMat = trans.getLocalMatrix();
	mBillboardMat.transpose();
}

void ObjEffect::OnUpdate()
{
	for (int i = 0; i < mBillboardCount; ++i)
	{
		if (mParamsBillboards->boards[i].refDiscard > 0)
		{
			mParamsBillboards->boards[i].refDiscard -= 0.01f;
			if (mParamsBillboards->boards[i].refDiscard < 0)
			{
				mParamsBillboards->boards[i].worldMat = Matrix4f();
				mParamsBillboards->boards[i].refDiscard = 0;
			}
		}
	}
}

void ObjEffect::LoadMesh()
{
	jMesh* mesh = new jMesh();
	Vector2 size(3, 3);
	Vector2 stepUV(0.125, 1);
	int RectCount = 20;
	vector<VertexFormat> verticies;
	vector<u32> indicies;
	for (int i = 0; i < RectCount; ++i)
	{
		VertexFormat vert[4];
		vert[0].position = Vector3(-size.x, 0, -size.y);
		vert[1].position = Vector3( size.x, 0, -size.y);
		vert[2].position = Vector3(-size.x, 0,  size.y);
		vert[3].position = Vector3( size.x, 0,  size.y);
		vert[0].texel = Vector2f(0			, stepUV.y);
		vert[1].texel = Vector2f(stepUV.x	, stepUV.y);
		vert[2].texel = Vector2f(0			, 0);
		vert[3].texel = Vector2f(stepUV.x	, 0);
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
