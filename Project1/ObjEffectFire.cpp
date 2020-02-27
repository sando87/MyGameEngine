#include "ObjEffectFire.h"
#include "jMesh.h"
#include "jImage.h"
#include "jTransform.h"
#include "ObjCamera.h"
#include "jShaderEffectTrace.h"
#include "jTime.h"

ObjEffectFire::ObjEffectFire()
{
}

ObjEffectFire::~ObjEffectFire()
{
}
void ObjEffectFire::OnLoad()
{
	LoadMesh();

	AddComponent(new jImage("./res/img/103_0000028EB659FE60_t.tga"));
	//AddComponent(new jImage("./res/img/103_0000028EBB2AF120_t.tga"));

	mShader = new jShaderEffectTrace();
	mShader->GetParamBasic().spriteStep = Vector2f(0.25f,1);
	mParamsBillboards = &mShader->GetParamBillboard();
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	AddComponent(mShader);

	mBillboardIndex = 0;
	mBillboardCount = sizeof(mParamsBillboards->boards) / sizeof(mParamsBillboards->boards[0]);

	mSizes.Init(Vector2(1, 10), Vector2(0, 0));
	mHeights.Init(30, 0);
}
void ObjEffectFire::OnStart()
{
	ObjCamera* cam = GetEngine().FindGameObject<ObjCamera>();
	jTransform trans;
	trans.lookat(Vector3(), cam->GetTransform().getView(), Vector3(0, 0, 1));
	mBillboardMat = trans.getLocalMatrix();
}
void ObjEffectFire::OnUpdate()
{
	static double time = 0;
	time += jTime::Delta();
	if (time < 3)
		return;
	else if (time > 4)
		SetRemove(true);

	double curHeight = mHeights.CalcYAcc(jTime::Delta());
	double curSize = mSizes.CalcYAcc(jTime::Delta());
	Matrix4 mat = Matrix4().identity().scale(curSize);
	mat *= mBillboardMat;
	mat[14] = curHeight;
	mParamsBillboards->boards[0].worldMat = mat.transpose();
	mParamsBillboards->boards[0].texelIndex = Vector2f(0, 0);
	mParamsBillboards->boards[0].reserve++;
	
}
void ObjEffectFire::LoadMesh()
{
	jMesh* mesh = new jMesh();
	Vector2 size(3, 3);
	Vector2 stepUV(0.25, 1);
	int RectCount = 20;
	vector<VertexFormat> verticies;
	vector<u32> indicies;
	for (int i = 0; i < RectCount; ++i)
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