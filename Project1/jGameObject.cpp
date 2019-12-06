#include "jGameObject.h"
#include "jGameObjectMgr.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"

jGameObject::jGameObject()
{
	mIsRemoved = false;
	mIsStarted = false;
}


jGameObject::~jGameObject()
{
}

void jGameObject::AddToMgr()
{
	jGameObjectMgr::GetInst().AddGameObject(this);
}

ObjCamera & jGameObject::GetCamera()
{
	return jGameObjectMgr::GetInst().GetCamera();
}
ObjTerrainMgr & jGameObject::GetTerrain()
{
	return jGameObjectMgr::GetInst().GetTerrain();
}

void jGameObject::OnStart()
{
}

void jGameObject::OnUpdate()
{
}

void jGameObject::OnDraw()
{
}
