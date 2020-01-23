#include "jComponent.h"



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
