#include "jLoader.h"


void jLoader::LoadObjFile(string _filename)
{
	ifstream input(_filename.c_str());
	if (input.fail())
	{
		_warn();
		return;
	}

	mObjects.push_back(ObjectInfo());
	ObjectInfo& info = mObjects.back();

	string line;
	while (getline(input, line))
	{
		vector<string> vec;
		jUtils::Split(line, " \n", vec);
		if (vec[0] == "v")
			info.Pos.push_back(Vector3f(stof(vec[1]), stof(vec[2]), stof(vec[3])));
		else if (vec[0] == "vt")
			info.UV.push_back(Vector2f(stof(vec[1]), stof(vec[2])));
		else if (vec[0] == "vn")
			info.Normal.push_back(Vector3f(stof(vec[1]), stof(vec[2]), stof(vec[3])));
		else if (vec[0] == "f")
		{
			int faceCnt = vec.size();
			for (int j = 1; j < faceCnt; ++j)
			{
				vector<string> vertS;
				jUtils::Split(vec[j], "/", vertS);
				info.VertexIdx.push_back(Vector3n(stoi(vertS[0]) - 1, stoi(vertS[1]) - 1, stoi(vertS[2]) - 1));
			}
		}
		else
			info.Info.push_back(line);
	}

	input.close();
}

void jLoader::LoadDaeFile(string _filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(_filename.c_str());

	XMLElement* ctrlNode = doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_controllers")
		->FirstChildElement("controller");

	xmlElements objs = GetObjects(doc);
	int count = objs->size();
	for (int i = 0; i < count; ++i)
	{
		XMLElement* ele = objs[i];

		mObjects.push_back(ObjectInfo());
		ObjectInfo& info = mObjects.back();
		ParseMeshes(ele, info);

		const char *val = nullptr;
		ele->QueryStringAttribute("id", &val);
		string objectName = string(val);

		XMLElement* currnet = ctrlNode;
		XMLElement* findSkinNode = nullptr;
		while (currnet != nullptr)
		{
			const char *src = nullptr;
			findSkinNode = currnet->FirstChildElement("skin");
			findSkinNode->QueryStringAttribute("source", &src);
			if ("#" + objectName == string(src))
				break;

			findSkinNode = nullptr;
			currnet = currnet->NextSiblingElement();
		}

		if(findSkinNode)
			ParseWeights(findSkinNode, info);
	}

	doc.Clear();
}


xmlElements jLoader::GetObjects(tinyxml2::XMLDocument& _doc)
{
	xmlElements eles;
	tinyxml2::XMLDocument& doc = _doc;

	if (string(doc.FirstChildElement()->Name()) != "COLLADA")
		return eles;

	XMLElement* geoEle = doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_geometries")
		->FirstChildElement("geometry");

	while (geoEle != nullptr)
	{
		eles->push_back(geoEle);
		geoEle = geoEle->NextSiblingElement();
	}

	return eles;
}
void jLoader::ParseMeshes(XMLElement* _ele, ObjectInfo & info)
{
	XMLElement* geoEle = _ele->FirstChildElement("mesh");

	int cntVec = 0;
	XMLElement* meshEle = geoEle->FirstChildElement("source");
	const char* points = meshEle->FirstChildElement("float_array")->GetText();
	vector<string> vecPoints;
	jUtils::Split(string(points), " \n", vecPoints);
	cntVec = vecPoints.size() / 3;
	for (int i = 0; i < cntVec; ++i)
	{
		Vector3f pt;
		pt.x = stof(vecPoints[i * 3 + 0]);
		pt.y = stof(vecPoints[i * 3 + 1]);
		pt.z = stof(vecPoints[i * 3 + 2]);
		info.Pos.push_back(pt);
	}

	meshEle = meshEle->NextSiblingElement();
	const char* normals = meshEle->FirstChildElement("float_array")->GetText();
	vector<string> vecNormals;
	jUtils::Split(string(normals), " \n", vecNormals);
	cntVec = vecNormals.size() / 3;
	for (int i = 0; i < cntVec; ++i)
	{
		Vector3f nr;
		nr.x = stof(vecNormals[i * 3 + 0]);
		nr.y = stof(vecNormals[i * 3 + 1]);
		nr.z = stof(vecNormals[i * 3 + 2]);
		info.Normal.push_back(nr);
	}

	meshEle = meshEle->NextSiblingElement();
	const char* uvs = meshEle->FirstChildElement("float_array")->GetText();
	vector<string> vecUVs;
	jUtils::Split(string(uvs), " \n", vecUVs);
	cntVec = vecUVs.size() / 2;
	for (int i = 0; i < cntVec; ++i)
	{
		Vector2f uv;
		uv.x = stof(vecUVs[i * 2 + 0]);
		uv.y = stof(vecUVs[i * 2 + 1]);
		info.UV.push_back(uv);
	}

	const char* faces = geoEle->FirstChildElement("triangles")->FirstChildElement("p")->GetText();
	vector<string> vecFaces;
	jUtils::Split(string(faces), " \n", vecFaces);
	cntVec = vecFaces.size() / 3;
	for (int i = 0; i < cntVec; ++i)
	{
		Vector3n vertN;
		vertN.x = stoi(vecFaces[i * 3 + 0]);
		vertN.z = stoi(vecFaces[i * 3 + 1]);
		vertN.y = stoi(vecFaces[i * 3 + 2]);

		info.VertexIdx.push_back(vertN);
	}
}
void jLoader::ParseWeights(tinyxml2::XMLElement* _ele, ObjectInfo & info)
{
	XMLElement* element = _ele->FirstChildElement("source");

	const char* boneNames = element->FirstChildElement("Name_array")->GetText();
	string boneNames_(boneNames);

	element = element->NextSiblingElement()->NextSiblingElement();
	const char* weights = element->FirstChildElement("float_array")->GetText();
	string weights_(weights);

	element = element->NextSiblingElement()->NextSiblingElement();
	const char* vc = element->FirstChildElement("vcount")->GetText();
	string vc_(vc);
	const char* v = element->FirstChildElement("v")->GetText();
	string v_(v);

	vector<string> vecNames;
	vector<string> vecWeights;
	vector<string> vecVCs;
	vector<string> vecVs;
	jUtils::Split(boneNames_, " \n", vecNames);
	jUtils::Split(weights_, " \n", vecWeights);
	jUtils::Split(vc_, " \n", vecVCs);
	jUtils::Split(v_, " \n", vecVs);

	int accIdx = 0;
	int cntVC = vecVCs.size();
	for (int i = 0; i < cntVC; ++i)
	{
		Vector4n bones;
		Vector4f weights;
		int cntV = stoi(vecVCs[i]);
		cntV = min(4, cntV);
		for (int j = 0; j < cntV; ++j)
		{
			bones[j] = stoi(vecVs[(accIdx + j) * 2]);
			int weightIdx = stoi(vecVs[(accIdx + j) * 2 + 1]);
			weights[j] = stof(vecWeights[weightIdx]);
		}
		accIdx += cntV;
		info.BoneIndexs.push_back(bones);
		info.Weights.push_back(weights);
	}
}

