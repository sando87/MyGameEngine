#include "jLoader.h"
#include "tinyxml2.h"
using namespace tinyxml2;

void jLoader::LoadObjFile(string _filename)
{
	ifstream input(_filename.c_str());
	if (input.fail())
	{
		_warn();
		return;
	}

	string line;
	while (getline(input, line))
	{
		vector<string> vec;
		jUtils::Split(line, " \n", vec);
		if (vec[0] == "v")
			mPos.push_back(Vector3f(stof(vec[1]), stof(vec[2]), stof(vec[3])));
		else if (vec[0] == "vt")
			mUV.push_back(Vector2f(stof(vec[1]), stof(vec[2])));
		else if (vec[0] == "vn")
			mNormal.push_back(Vector3f(stof(vec[1]), stof(vec[2]), stof(vec[3])));
		else if (vec[0] == "f")
		{
			int faceCnt = vec.size();
			for (int j = 1; j < faceCnt; ++j)
			{
				vector<string> vertS;
				jUtils::Split(vec[j], "/", vertS);
				mVertexIdx.push_back(Vector3n(stoi(vertS[0]) - 1, stoi(vertS[1]) - 1, stoi(vertS[2]) - 1));
			}
		}
		else
			mInfo.push_back(line);
	}

	input.close();
}

void jLoader::LoadDaeFile(string _filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(_filename.c_str());

	if (string(doc.FirstChildElement()->Name()) != "COLLADA")
		return;


	XMLElement* geoEle = doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_geometries")
		->FirstChildElement("geometry")
		->FirstChildElement("mesh");

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
		mPos.push_back(pt);
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
		mNormal.push_back(nr);
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
		mUV.push_back(uv);
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

		mVertexIdx.push_back(vertN);
	}

	XMLElement* element = doc.FirstChildElement("COLLADA")
		->FirstChildElement("library_controllers")
		->FirstChildElement("controller")
		->FirstChildElement("skin")
		->FirstChildElement("source");

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
		mBoneIndexs.push_back(bones);
		mWeights.push_back(weights);
	}

}

