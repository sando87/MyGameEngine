#include "cUserInputDriven.h"
#include "oUserInputDriver.h"
#include "jGameObjectMgr.h"


cUserInputDriven::cUserInputDriven()
{
}


cUserInputDriven::~cUserInputDriven()
{
}

void cUserInputDriven::OnLoad()
{
	if (EventMouseDown ||
		EventMouseUp ||
		EventMouseMove ||
		EventMouseClick ||
		EventMouseDrag ||
		EventMouseWheel ||
		EventKeyDown ||
		EventKeyUp ||
		EventKeyPressed)
	{
		oUserInputDriver* userInputDriver = GetEngine().FindGameObject<oUserInputDriver>();
		userInputDriver->AddDriven(this);
	}
}
