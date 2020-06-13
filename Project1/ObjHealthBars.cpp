#include "ObjHealthBars.h"
#include "jMesh.h"
#include "jShaderHPBar.h"
#include "jImage.h"
#include "ObjCamera.h"
#include "jTransform.h"
#include "jHealthPoint.h"
#include "jTime.h"

void ObjHealthBars::OnLoad()
{
	jMesh* mesh = CreateHpMesh(40);
	AddComponent(mesh);

	AddComponent(new jImage("./res/img/hpbar.png"));

	mShader = new jShaderHPBar();
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	mShader->SetRenderOrder(RenderOrder_Terrain_Env_Alpha);
	AddComponent(mShader);
}

void ObjHealthBars::OnUpdate()
{
	ShaderBufferHPBars& param = mShader->GetParamBars();
	memset(param.bars, 0x00, sizeof(param.bars));

	int idx = 0;
	for (auto iter = mHPBars.begin(); iter != mHPBars.end();)
	{
		HpDrawInfo& info = iter->second;
		Vector3 pos = info.obj->GetTransform().getPos();
		pos.z += 7.0;
		double refDiscard = info.hp->CurSpec.hp / info.hp->MaxSpec.hp; //hp Rate
		info.accTime += jTime::Delta();
		bool isDeath = (info.lifeTime < info.accTime) ? true : false;

		if (isDeath)
		{
			mHPBars.erase(++iter);
		}
		else
		{
			param.bars[idx].transform.x = pos.x;
			param.bars[idx].transform.y = pos.y;
			param.bars[idx].transform.z = pos.z;
			param.bars[idx].hpRate = refDiscard;
			idx++;
			++iter;
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

void ObjHealthBars::ShowHPBar(jHealthPoint* hp)
{
	HpDrawInfo info;
	info.obj = hp->GetGameObject();
	info.hp = hp;
	info.lifeTime = 5;
	info.accTime = 0;
	mHPBars[hp] = info;
}

void ObjHealthBars::DeleteHPBar(jHealthPoint * hp)
{
	if (mHPBars.find(hp) != mHPBars.end())
		mHPBars.erase(hp);
}