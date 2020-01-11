#include "jGameObject.h"
#include "jGameObjectMgr.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jComponent.h"
#include "jMatrixControl.h"
#include "jShader.h"
#include "jAnimCSV.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "jShaderTerrain.h"
#include "jShaderDefault.h"
#include "jObjectMeta.h"


jGameObject::jGameObject()
{
	mIsRemoved = false;
	mIsStarted = false;
	mTransport = new jMatrixControl();
	mTransport->init();
}


jGameObject::~jGameObject()
{
	if (mTransport)
		delete mTransport;
}

void jGameObject::AddToMgr()
{
	jGameObjectMgr::GetInst().AddGameObject(this);
}

jMatrixControl & jGameObject::GetTransport()
{
	return *mTransport;
}
ObjCamera & jGameObject::GetCamera()
{
	jGameObjectMgr& inst = jGameObjectMgr::GetInst();
	return inst.GetCamera();
	//return jGameObjectMgr::GetInst().GetCamera();
}
ObjTerrainMgr & jGameObject::GetTerrain()
{
	return jGameObjectMgr::GetInst().GetTerrain();
}
void jGameObject::StopCoRoutine(string name)
{
	jGameObjectMgr::GetInst().StopCoroutine(name);
}
void jGameObject::StartCoRoutine(string name, std::function<CoroutineReturn(void)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CoroutineMode::Normal;
	info.name = name;
	info.coroutine = coroutine;
	jGameObjectMgr::GetInst().StartCoroutine(info);
}
void jGameObject::StartCoRoutine(string name, float time_ms, std::function<CoroutineReturn(void)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CoroutineMode::Timer;
	info.name = name;
	info.time_ms = time_ms;
	info.time_back_ms = time_ms;
	info.coroutine = coroutine;
	jGameObjectMgr::GetInst().StartCoroutine(info);
}
void jGameObject::StartCoRoutine(string name, std::function<void(void)> task, std::function<CoroutineReturn(void)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CoroutineMode::Task;
	info.name = name;
	info.task = task;
	info.taskStarted = false;
	info.taskDone = false;
	info.coroutine = coroutine;
	jGameObjectMgr::GetInst().StartCoroutine(info);
}
bool jGameObject::LoadTxt(string objName)
{
	jObjectMeta metaInfo;
	if (metaInfo.Load(objName) == false)
		return false;

	AddComponent(new jAnimCSV(metaInfo.GetAnimFullName()));
	AddComponent(new jMesh(metaInfo.GetObjFullName()));
	for(int i = 0; i < metaInfo.imgFileNames.size(); ++i)
		AddComponent(new jImage(metaInfo.GetImgFullName(i)));

	if (metaInfo.type_shader == "terrain")
	{
		jShaderTerrain* shader = new jShaderTerrain();
		shader->SetAlphaOn(metaInfo.alpha);
		shader->SetDepthOn(metaInfo.depth);
		ShaderParamsTerrain& param = shader->GetParams();
		memcpy(param.vectors, &metaInfo.texels[0], sizeof(Vector4f) * metaInfo.texels.size());
		AddComponent(shader);
	}
	else if (metaInfo.type_shader == "default")
	{
		jShaderDefault* shader = new jShaderDefault();
		shader->SetAlphaOn(metaInfo.alpha);
		shader->SetDepthOn(metaInfo.depth);
		ShaderParamsDefault& param = shader->GetParams();
		param.material.diffuse = Vector4f(1, 1, 1, 1);
		param.light.direction = Vector4f(-1, -1, -1, 0);
		if(metaInfo.alpha == false) //ObjCreateHeightMap 생성시 주석해제필요
		AddComponent(shader);
	}
	else if (metaInfo.type_shader == "skin")
		AddComponent(new jShaderSkin());

	GetTransport().moveTo(metaInfo.worldPos);
	return true;
}
void jGameObject::AddComponent(jComponent* comp)
{
	mComponents.push_back(comp);
	comp->mGameObject = this;
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
