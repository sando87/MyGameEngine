#include "ObjGroundAxis.h"
#include "jMesh.h"
#include "ObjCamera.h"
#include "jShaderColor.h"

ObjGroundAxis::ObjGroundAxis()
{
}


ObjGroundAxis::~ObjGroundAxis()
{
}

void ObjGroundAxis::OnStart()
{
	jMesh* mesh = new jMesh();
	//mesh->LoadGrid(-100, 100, 200, 200, 10);
	//AddComponent(mesh);

	mesh = new jMesh();
	mesh->LoadAxis(10);
	AddComponent(mesh);

	AddComponent(new jShaderColor());
}

