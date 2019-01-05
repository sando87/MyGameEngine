#include "jGameObject.h"
#include "jGameObjectMgr.h"
#include "jCamera.h"

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

jCamera & jGameObject::GetCamera()
{
	return jGameObjectMgr::GetInst().GetCamera();
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
