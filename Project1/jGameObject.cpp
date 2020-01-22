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
#include "jObjectMeta.h"


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

bool jGameObject::LoadTxt(string objName)
{
	jObjectMeta metaInfo;
	if (metaInfo.Load(objName) == false)
		return false;

	AddComponent(new jAnimator(metaInfo.GetAnimFullName()));
	AddComponent(new jMesh(metaInfo.GetObjFullName()));
	for(int i = 0; i < metaInfo.imgFileNames.size(); ++i)
		AddComponent(new jImage(metaInfo.GetImgFullName(i)));

	if (metaInfo.type_shader == "terrain")
	{
		jShaderTerrain* shader = new jShaderTerrain();
		shader->SetAlphaOn(metaInfo.alpha);
		shader->SetDepthOn(metaInfo.depth);
		shader->SetRenderOrder(metaInfo.renderingOrder);
		ShaderParamsTerrain& param = shader->GetParams();
		memcpy(param.vectors, &metaInfo.texels[0], sizeof(Vector4f) * metaInfo.texels.size());
		AddComponent(shader);
	}
	else if (metaInfo.type_shader == "default")
	{
		jShaderDefault* shader = new jShaderDefault();
		shader->SetAlphaOn(metaInfo.alpha); //ObjCreateHeightMap 积己矫 false
		shader->SetDepthOn(metaInfo.depth); //ObjCreateHeightMap 积己矫 true
		shader->SetRenderOrder(metaInfo.renderingOrder);
		ShaderParamsDefault& param = shader->GetParams();
		param.material.diffuse = Vector4f(1, 1, 1, 1);
		param.light.direction = Vector4f(-1, -1, -1, 0);
		AddComponent(shader);
	}
	else if (metaInfo.type_shader == "skin")
	{
		jShaderSkin* shader = new jShaderSkin();
		shader->SetRenderOrder(metaInfo.renderingOrder);
		AddComponent(shader);
	}
		

	GetTransport().moveTo(metaInfo.worldPos);
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
