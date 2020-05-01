#include "ObjTerrain.h"
#include "jShaderTerrain.h"
#include "jHeightMap.h"
#include "jMesh.h"
#include "jImage.h"
#include "jTransform.h"
#include "jShaderDefault.h"
#include "jShaderColor.h"
#include "cHeightMap.h"
#include "cCollider.h"
#include "jParserMeta.h"
#include "cUserInputDriven.h"

#define META_OBJNAME 0
#define META_SHADERNAME 1
#define META_ALPHA 2
#define META_DEPTH 3
#define META_WORLD 4
#define META_ANIMATION 5
#define META_TEXTURES 6

//#define EditMode

ObjTerrain::ObjTerrain()
{
}


ObjTerrain::~ObjTerrain()
{
}

void ObjTerrain::Load(string filename)
{
	mMetaFullname = PATH_RESOURCES + string("meta/") + filename;
	jGameObject::LoadTxt(mMetaFullname);

	string type = GetTypeTxt();
	if (type == "heightmap")
		AddComponent(new cHeightMap());
	else if (type == "collider")
		AddComponent(new cColliderGridPole());
	else if (type.length() == 0)
	{
		if (nullptr != FindComponent<jShaderDefault>())
			AddComponent(new cColliderGridPole());
		else if(nullptr != FindComponent<jShaderTerrain>())
			AddComponent(new cHeightMap());
	}

#ifdef EditMode
	if (nullptr != FindComponent<cColliderGridPole>())
	{
		cUserInputDriven* driven = new cUserInputDriven();
		driven->EventHoverEnter = [&]() { FindComponent<jShaderDefault>()->GetParamBasic().matDiffuse = Vector4f(0, 0, 0, 1); };
		driven->EventHoverLeave = [&]() { FindComponent<jShaderDefault>()->GetParamBasic().matDiffuse = Vector4f(1, 1, 1, 1); };
		driven->EventSelected = [&]() { Show(true); };
		driven->EventUnSelected = [&]() { Show(false); };
		driven->EventKeyDown = [&](char key) { OnKeyDown(key); };
		AddComponent(driven);

		CreateChildCollisionBox();
	}
#endif
		
	return;
}

void ObjTerrain::OnStart()
{
	cColliderGridPole* collider = FindComponent<cColliderGridPole>();
	if (nullptr != collider && nullptr != mChild)
	{
		jRect3D box = collider->GetShape()->GetBox();
		Vector3 localPos = box.Center() - GetTransform().getPos();
		mChild->GetTransform().moveTo(localPos);
		mChild->GetTransform().Zoom(box.Size());
		Show(false);
	}
}

bool ObjTerrain::SetTypeTxt(string value)
{
	jParserMeta parse;
	bool ret = parse.Load(mMetaFullname);
	_exceptif(!ret, return false);

	parse.SetValue(MF_Type, value);
	parse.Save(mMetaFullname);
	return true;
}
string ObjTerrain::GetTypeTxt()
{
	jParserMeta parse;
	bool ret = parse.Load(mMetaFullname);
	_exceptif(!ret, return "");

	return parse.GetValue(MF_Type);
}

void ObjTerrain::CreateChildCollisionBox()
{
	mChild = new jGameObject();
	jMesh* mesh = new jMesh();
	mesh->LoadCubeOutline(1);
	mChild->AddComponent(mesh);
	mChild->AddComponent(new jShaderColor());
	AddChild(mChild);
}

void ObjTerrain::Show(bool isShow)
{
	if(mChild)
		mChild->FindComponent<jShaderColor>()->SetEnable(isShow);
}

void ObjTerrain::OnKeyDown(char key)
{
	string fullname = mMetaFullname;
	_echoS("editted filename : " + fullname);
	switch (key)
	{
	case 'T': //terrain
	case 'H':
		SetTypeTxt("heightmap");
		FindComponent<cCollider>()->SetEnableCollision(false);
		_echoS("enable heightmap");
		break;
	case 'C': //collider
		SetTypeTxt("collider");
		FindComponent<cCollider>()->SetEnableCollision(true);
		_echoS("enable collider");
		break;
	case 'N': //none
		SetTypeTxt("nothing");
		FindComponent<cCollider>()->SetEnableCollision(false);
		_echoS("enable nothing");
		break;
	case 'R': //remove
		jUtils::DeleteFullname(fullname);
		Show(false);
		FindComponent<jShaderDefault>()->SetEnable(false);
		FindComponent<cCollider>()->SetEnableCollision(false);
		_echoS("delete feature");
		break;
	case 'V': //visible
		FindComponent<jShaderDefault>()->SetEnable(!FindComponent<jShaderDefault>()->GetEnable());
		_echoS("toggle visible");
		break;
	default:
		break;
	}
}
