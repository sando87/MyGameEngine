#include "jGameObject.h"
#include "jGameObjectMgr.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jComponent.h"
#include "jMatrixControl.h"
#include "jShader.h"

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
void jGameObject::StartCoRoutine(std::function<bool(void)> coroutine, string name)
{
	if(name.length() > 0)
		jGameObjectMgr::GetInst().StartCoroutine(coroutine, name);
	else
		jGameObjectMgr::GetInst().StartCoroutine(coroutine);
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

void jGameObject::OnDraw()
{
	jShader* shader = FindComponent<jShader>();
	if (shader != nullptr && shader->GetLoaded() && shader->GetEnabled() && shader->GetVisiable())
		shader->Render();
}
