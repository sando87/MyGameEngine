#include "ObjTerrain.h"
#include "jShaderTerrain.h"
#include "jHeightMap.h"
#include "jMesh.h"
#include "jImage.h"
#include "jMatrixControl.h"

ObjTerrain::ObjTerrain()
{
}


ObjTerrain::~ObjTerrain()
{
}

void ObjTerrain::Load(string name, float size, float step, jHeightMap* heightmap)
{
	string metaPath = PATH_RESOURCES + string("meta\\");
	string meshPath = PATH_RESOURCES + string("mesh\\");
	string imgPath = PATH_RESOURCES + string("img\\");
	strings lines = jUtils::LoadLines(metaPath + name + ".txt");

	jMesh* mesh = new jMesh();
	mesh->Load(meshPath + lines[0] + ".obj");

	strings pieces = jUtils::Split2(lines[1], " ");
	Vector3 position = Vector3(jUtils::ToDouble(pieces[0]), jUtils::ToDouble(pieces[1]), jUtils::ToDouble(pieces[2]));
	position.x = ((int)position.x / size) * size;
	position.y = ((int)position.y / size) * size;
	mTransport->moveTo(position);

	heightmap->UpdateHeightMap(mesh, position);

	vector<jImage*> textures;
	vector<Vector4f> texels;
	for (int i = 2; i < lines->size(); ++i)
	{
		strings columns = jUtils::Split2(lines[i], " ");
		if (columns->size() >= 3)
		{
			Vector4f pt;
			pt.x = jUtils::ToDouble(columns[0]);
			pt.y = jUtils::ToDouble(columns[1]);
			pt.z = jUtils::ToDouble(columns[2]);
			pt.w = 0;
			texels.push_back(pt);
		}
		else
		{
			string tgaImageName = columns[0];
			jImage* texture = new jImage();
			texture->Load(imgPath + tgaImageName);
			textures.push_back(texture);
		}
	}

	jShaderTerrain* shader = new jShaderTerrain();
	shader->SetTextures(textures);
	ShaderParamsTerrain& param = shader->GetParams();
	memcpy(param.vectors, &texels[0], sizeof(Vector4f) * texels.size());

	AddComponent(shader);
	AddComponent(mesh);
	
	return;
}

