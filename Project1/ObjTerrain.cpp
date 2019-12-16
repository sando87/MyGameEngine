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
	string path = "E:\\export\\D3\\" + name + "\\";

	jMesh* mesh = new jMesh();
	mesh->Load(path + name + ".obj");

	strings lines = jUtils::LoadLines(path + name + ".txt");
	strings pieces = jUtils::Split2(lines[0], " ");
	Vector3 position = Vector3(jUtils::ToDouble(pieces[0]), jUtils::ToDouble(pieces[1]), jUtils::ToDouble(pieces[2]));
	position.x = ((int)position.x / size) * size;
	position.y = ((int)position.y / size) * size;
	mTransport->moveTo(position);

	heightmap->UpdateHeightMap(mesh, position);

	vector<jImage*> textures;
	vector<Vector4f> texels;
	for (int i = 1; i < lines->size(); ++i)
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
			texture->Load(path + tgaImageName);
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

