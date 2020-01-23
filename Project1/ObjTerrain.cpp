#include "ObjTerrain.h"
#include "jShaderTerrain.h"
#include "jHeightMap.h"
#include "jMesh.h"
#include "jImage.h"
#include "jTransform.h"
#include "jShaderDefault.h"

#define META_OBJNAME 0
#define META_SHADERNAME 1
#define META_ALPHA 2
#define META_DEPTH 3
#define META_WORLD 4
#define META_ANIMATION 5
#define META_TEXTURES 6

ObjTerrain::ObjTerrain()
{
}


ObjTerrain::~ObjTerrain()
{
}

void ObjTerrain::Load(string name)
{
	jGameObject::LoadTxt(name);
	return;
}

