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



struct ObjMetaInfo
{
	string filename;
	string objname;
	string type_shader;
	bool alpha;
	bool depth;
	Vector3 worldPos;
	string animName;
	vector<Vector4f> texels;
	vector<string> imgFileNames;
	bool Load(string name)
	{
		string metaPath = PATH_RESOURCES + string("meta/");
		strings lines = jUtils::LoadLines(metaPath + name);
		if (!lines)
			return false;

		filename = name;
		objname = lines[0];
		type_shader = lines[1];
		alpha = lines[2] == "TRUE" ? true : false;
		depth = lines[3] == "TRUE" ? true : false;

		strings pieces = jUtils::Split2(lines[4], " ");
		Vector3 position;
		position.x = jUtils::ToDouble(pieces[0]);
		position.y = jUtils::ToDouble(pieces[1]);
		position.z = jUtils::ToDouble(pieces[2]);
		worldPos = position;

		animName = lines[5];

		for (int i = 6; i < lines->size(); ++i)
		{
			strings columns = jUtils::Split2(lines[i], " ");
			if (columns->size() >= 3)
			{
				Vector4f pt;
				pt.x = jUtils::ToDouble(columns[0]);
				pt.y = jUtils::ToDouble(columns[1]);
				pt.z = jUtils::ToDouble(columns[2]);
				pt.w = 0;
				texels.push_back(pt);
			}
			else
			{
				imgFileNames.push_back(columns[0]);
			}
		}

		return true;
	}
	string GetObjFullName() { return PATH_RESOURCES + string("mesh/") + objname; }
	string GetImgFullName(int idx) { return PATH_RESOURCES + string("img/") + imgFileNames[idx]; }
	string GetAnimFullName() { return PATH_RESOURCES + string("anim/") + animName; }

};


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
	return jGameObjectMgr::GetInst().GetCamera();
}
ObjTerrainMgr & jGameObject::GetTerrain()
{
	return jGameObjectMgr::GetInst().GetTerrain();
}
void jGameObject::StopCoRoutine(string name)
{
	jGameObjectMgr::GetInst().StopCoroutine(name);
}
void jGameObject::StartCoRoutine(std::function<CoroutineReturn(void)> coroutine, string name)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CoroutineMode::Normal;
	info.name = name;
	info.coroutine = coroutine;
	jGameObjectMgr::GetInst().StartCoroutine(info, name);
}
void jGameObject::StartCoRoutine(std::function<CoroutineReturn(void)> coroutine, float time_ms, string name)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CoroutineMode::Timer;
	info.name = name;
	info.time_ms = time_ms;
	info.time_back_ms = time_ms;
	info.coroutine = coroutine;
	jGameObjectMgr::GetInst().StartCoroutine(info, name);
}
void jGameObject::StartCoRoutine(std::function<CoroutineReturn(void)> coroutine, std::function<void(void)> task, string name)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CoroutineMode::Task;
	info.name = name;
	info.task = task;
	info.taskStarted = false;
	info.taskDone = false;
	info.coroutine = coroutine;
	jGameObjectMgr::GetInst().StartCoroutine(info, name);
}
bool jGameObject::LoadTxt(string objName)
{
	ObjMetaInfo metaInfo;
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
