#include "cCuiDriven.h"
#include "oCuiDriver.h"
#include "jGameObjectMgr.h"

cCuiDriven::cCuiDriven()
{
}


cCuiDriven::~cCuiDriven()
{
	GetEngine().FindGameObject<oCuiDriver>()->SubDriven(this);
}

void cCuiDriven::OnStart()
{
	GetEngine().FindGameObject<oCuiDriver>()->AddDriven(this);
}
