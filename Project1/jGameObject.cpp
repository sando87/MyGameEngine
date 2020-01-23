#include "jGameObject.h"
#include "jGameObjectMgr.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jComponent.h"
#include "jMatrixControl.h"
#include "jShader.h"
#include "jAnimator.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "jShaderTerrain.h"
#include "jShaderDefault.h"
#include "jParserMeta.h"


jGameObject::jGameObject()
{
	mTransport = new jMatrixControl();
	mEngine = &jGameObjectMgr::GetInst();
}


jGameObject::~jGameObject()
{
	if (mTransport)
		delete mTransport;
}

jMatrixControl & jGameObject::GetTransport()
{
	return *mTransport;
}

jGameObjectMgr & jGameObject::GetEngine()
{
	return *mEngine;
}
bool jGameObject::LoadTxt(string filename)
{
	jParserMeta parse;
	bool ret = parse.Load(PATH_RESOURCES + string("meta/") + filename);
	if (!ret)
	{
		_warn();
		return false;
	}

	mName = parse.GetValue("name");
	string filenameObj = parse.GetValue("mesh");
	vector<string> filenameImgs = parse.GetValues("img");
	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + filenameObj));
	for(int i = 0; i < filenameImgs.size(); ++i)
		AddComponent(new jImage(PATH_RESOURCES + string("img/") + filenameImgs[i]));

	string shaderType = parse.GetValue("shader");
	if (shaderType == "terrain")
	{
		jShaderTerrain* shader = new jShaderTerrain();
		shader->SetAlphaOn(parse.GetValue<bool>("alpha"));
		shader->SetDepthOn(parse.GetValue<bool>("depth"));
		shader->SetRenderOrder(parse.GetValue<double>("order"));
		vector<Vector3> texels = parse.GetValues<Vector3>("texel");
		ShaderParamsTerrain& param = shader->GetParams();
		int cnt = min(sizeof(param.vectors) / sizeof(param.vectors[0]), texels.size());
		for (int i = 0; i < cnt; ++i)
		{
			param.vectors[i].x = texels[i].x;
			param.vectors[i].y = texels[i].y;
			param.vectors[i].z = texels[i].z;
			param.vectors[i].w = 0;
		}
		AddComponent(shader);
	}
	else if (shaderType == "default")
	{
		jShaderDefault* shader = new jShaderDefault();
		shader->SetAlphaOn(parse.GetValue<bool>("alpha")); //ObjCreateHeightMap 积己矫 false
		shader->SetDepthOn(parse.GetValue<bool>("depth")); //ObjCreateHeightMap 积己矫 true
		shader->SetRenderOrder(parse.GetValue<double>("order"));
		ShaderParamsDefault& param = shader->GetParams();
		param.material.diffuse = Vector4f(1, 1, 1, 1);
		param.light.direction = Vector4f(-1, -1, -1, 0);
		AddComponent(shader);
	}
	else if (shaderType == "skin")
	{
		jShaderSkin* shader = new jShaderSkin();
		shader->SetRenderOrder(parse.GetValue<double>("order"));
		AddComponent(shader);
	}

	string filenameAnim = parse.GetValue("anim");
	AddComponent(new jAnimator(PATH_RESOURCES + string("anim/") + filenameAnim));

	GetTransport().moveTo(parse.GetValue<Vector3>("worldPos"));
	return true;
}
void jGameObject::StandOnTerrain()
{
	Vector3 pos = GetTransport().getPos();
	float height = 0;
	bool ret = mEngine->GetTerrain().GetHeight(pos.x, pos.y, height);
	if (ret)
	{
		pos.z = height;
		GetTransport().moveTo(pos);
	}
}
void jGameObject::AddChild(jGameObject* child)
{
	mChilds.push_back(child);
	child->mParent = this;
}
Matrix4 jGameObject::GetWorldMat()
{
	Matrix4 parentMat = mParent != nullptr ? mParent->GetWorldMat() : Matrix4();
	return GetTransport().getMatrix() * parentMat;
}
void jGameObject::AddComponent(jComponent* comp)
{
	comp->mGameObject = this;
	comp->OnLoad();
	mComponents.push_back(comp);
}
void jGameObject::RemoveComponent(jComponent* comp)
{
	for (jComponent* ele : mComponents)
	{
		if (ele == comp)
		{
			mComponents.remove(ele);
			break;
		}
	}
}

void jGameObject::OnStart()
{
}

void jGameObject::OnUpdate()
{
	UpdateComponents();
}

void jGameObject::LoadComponents()
{
	for (jComponent* comp : mComponents)
		if (comp->GetEnable())
			comp->OnLoad();
}

void jGameObject::UpdateComponents()
{
	for (jComponent* comp : mComponents)
		if (comp->GetEnable())
			comp->OnUpdate();
}
