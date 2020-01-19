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
	mRemove = false;
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
void jGameObject::StartCoRoutine(string name, std::function<CorCmd(CorMember&, bool)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CorMode::Normal;
	info.name = name;
	info.firstCalled = true;
	info.coroutine = coroutine;
	info.pThread = nullptr;
	jGameObjectMgr::GetInst().StartCoroutine(info);
}
void jGameObject::StartCoRoutine(string name, float time_ms, std::function<CorCmd(CorMember&, bool)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CorMode::Timer;
	info.name = name;
	info.time_ms = time_ms;
	info.time_back_ms = time_ms;
	info.firstCalled = true;
	info.coroutine = coroutine;
	info.pThread = nullptr;
	jGameObjectMgr::GetInst().StartCoroutine(info);
}

//name : 코루틴 이름, task : 쓰레드에서 수행되는 함수, coroutine : 쓰레드 완료시 수행되는 코루틴
void jGameObject::StartCoRoutine(string name, std::function<void(void)> task, std::function<CorCmd(CorMember&, bool)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CorMode::Task;
	info.name = name;
	info.task = task;
	info.taskStarted = false;
	info.taskDone = false;
	info.firstCalled = true;
	info.coroutine = coroutine;
	info.pThread = nullptr;
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
		shader->SetRenderOrder(metaInfo.renderingOrder);
		ShaderParamsTerrain& param = shader->GetParams();
		memcpy(param.vectors, &metaInfo.texels[0], sizeof(Vector4f) * metaInfo.texels.size());
		AddComponent(shader);
	}
	else if (metaInfo.type_shader == "default")
	{
		jShaderDefault* shader = new jShaderDefault();
		shader->SetAlphaOn(metaInfo.alpha); //ObjCreateHeightMap 생성시 false
		shader->SetDepthOn(metaInfo.depth); //ObjCreateHeightMap 생성시 true
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
	bool ret = GetTerrain().GetHeight(pos.x, pos.y, height);
	if (ret)
	{
		pos.z = height;
		GetTransport().moveTo(pos);
	}
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
