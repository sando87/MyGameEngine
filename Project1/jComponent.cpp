#include "jComponent.h"
#include "jGameObjectMgr.h"
#include "jTypeToString.h"

jComponent::jComponent()
{
}


jComponent::~jComponent()
{
}

void jComponent::Load()
{
	if (!mLoaded)
		OnLoad();
	mLoaded = true;
}

jGameObjectMgr& jComponent::GetEngine()
{
	return *mEngine;
}
