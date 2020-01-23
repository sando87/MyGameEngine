#include "ObjGroundAxis.h"
#include "jMesh.h"
#include "jShaderColor.h"
#include "jCrash.h"

ObjGroundAxis::ObjGroundAxis()
{
}


ObjGroundAxis::~ObjGroundAxis()
{
}

void ObjGroundAxis::OnLoad()
{
	jMesh* mesh = new jMesh();
	mesh->LoadGrid(-100, 100, 200, 200, 10);
	mesh->LoadAxis(10);
	AddComponent(mesh);

	jShaderColor* shader = new jShaderColor();
	AddComponent(shader);
}


