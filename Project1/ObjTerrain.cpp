#include "ObjTerrain.h"
#include "jShaderTerrain.h"
#include "jHeightMap.h"
#include "jMesh.h"
#include "jImage.h"
#include "jMatrixControl.h"
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
	//string metaPath = PATH_RESOURCES + string("meta/");
	//string meshPath = PATH_RESOURCES + string("mesh/");
	//string imgPath = PATH_RESOURCES + string("img/");
	//strings lines = jUtils::LoadLines(metaPath + name);
	//if (lines->size() < 5)
	//	return;
	//
	//if (lines[META_SHADERNAME] != "terrain" && lines[META_SHADERNAME] != "default")
	//	return;
	//
	//jMesh* mesh = new jMesh();
	//mesh->Load(meshPath + lines[META_OBJNAME]);
	//AddComponent(mesh);
	//
	//if (lines[META_SHADERNAME] == "terrain")
	//{
	//	jShaderTerrain* shader = new jShaderTerrain();
	//	vector<jImage*> textures;
	//	vector<Vector4f> texels;
	//	for (int i = META_TEXTURES; i < lines->size(); ++i)
	//	{
	//		strings columns = jUtils::Split2(lines[i], " ");
	//		if (columns->size() >= 3)
	//		{
	//			Vector4f pt;
	//			pt.x = jUtils::ToDouble(columns[0]);
	//			pt.y = jUtils::ToDouble(columns[1]);
	//			pt.z = jUtils::ToDouble(columns[2]);
	//			pt.w = 0;
	//			texels.push_back(pt);
	//		}
	//		else
	//		{
	//			string tgaImageName = columns[0];
	//			jImage* texture = new jImage();
	//			texture->Load(imgPath + tgaImageName);
	//			textures.push_back(texture);
	//		}
	//	}
	//
	//	shader->SetTextures(textures);
	//	shader->SetAlphaOn(lines[META_ALPHA] == "TRUE" ? true : false);
	//	shader->SetDepthOn(lines[META_DEPTH] == "TRUE" ? true : false);
	//	ShaderParamsTerrain& param = shader->GetParams();
	//	memcpy(param.vectors, &texels[0], sizeof(Vector4f) * texels.size());
	//
	//	AddComponent(shader);
	//}
	//else if (lines[META_SHADERNAME] == "default")
	//{
	//	jShaderDefault* shader = new jShaderDefault();
	//	ShaderParamsDefault& param = shader->GetParams();
	//	param.material.diffuse = Vector4f(1,1,1,1);
	//	param.light.direction = Vector4f(-1, -1, -1, 0);
	//	shader->SetAlphaOn(lines[META_ALPHA] == "TRUE" ? true : false);
	//	shader->SetDepthOn(lines[META_DEPTH] == "TRUE" ? true : false);
	//
	//	jImage* texture = new jImage();
	//	texture->Load(imgPath + lines[lines->size() - 1]);
	//
	//	AddComponent(texture);
	//	AddComponent(shader);
	//}
	//
	//strings pieces = jUtils::Split2(lines[META_WORLD], " ");
	//Vector3 position;
	//position.x = jUtils::ToDouble(pieces[0]);
	//position.y = jUtils::ToDouble(pieces[1]);
	//position.z = jUtils::ToDouble(pieces[2]);
	//mTransport->moveTo(position);
	
	return;
}

