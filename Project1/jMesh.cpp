#include "jMesh.h"
#include "jLoader.h"


jMesh::jMesh(string _name)
{
	if (_name.length() > 0)
		Load(_name);
}


jMesh::~jMesh()
{
}

bool jMesh::Load(string _name)
{
	jLoader data;
	if (jUtils::GetFileExtension(_name) == "obj")
	{
		data.LoadObjFile(_name);
	}
	else if (jUtils::GetFileExtension(_name) == "DAE")
	{
		data.LoadDaeFile(_name);
	}

	ObjectInfo& info = data.mObjects[0];
	int cnt = info.VertexIdx.size();
	for (int i = 0; i < cnt; ++i)
	{
		VertexFormat vertex;
		Vector3n& vertIdx = info.VertexIdx[i];
		vertex.position = info.Pos[vertIdx.x];
		vertex.texel = info.UV[vertIdx.y];
		vertex.normal = info.Normal[vertIdx.z];
		vertex.boneIndexs = info.BoneIndexs.size() > 0 ? info.BoneIndexs[vertIdx.x] : Vector4n();
		vertex.weights = info.Weights.size() > 0 ? info.Weights[vertIdx.x] : Vector4f(1,0,0,0);

		mVerticies.push_back(vertex);

		if (mVerticies.size() % 3 == 2)
		{
			int startIdx = mVerticies.size();
			mIndicies.push_back(startIdx);
			mIndicies.push_back(startIdx - 2);
			mIndicies.push_back(startIdx - 1);
		}
	}

	mName = _name;
	return true;
}
bool jMesh::LoadGrid(int _x, int _y, int _w, int _h, int _step)
{
	int left = _x;
	int top = _y;
	int right = _x + _w;
	int bottom = _y - _h;
	for (int i = left; i <= right; i += _step)
	{
		if (i == 0)
			continue;

		VertexFormat vert;
		vert.color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		vert.position = Vector3f(i, top, 0.0f);
		mVerticies.push_back(vert);

		vert.position = Vector3f(i, bottom, 0.0f);
		mVerticies.push_back(vert);

		int cnt = mVerticies.size();
		mIndicies.push_back(cnt - 2);
		mIndicies.push_back(cnt - 1);
	}

	for (int i = top; i >= bottom; i -= _step)
	{
		if (i == 0)
			continue;

		VertexFormat vert;
		vert.color = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
		vert.position = Vector3f(left, i, 0.0f);
		mVerticies.push_back(vert);

		vert.position = Vector3f(right, i, 0.0f);
		mVerticies.push_back(vert);

		int cnt = mVerticies.size();
		mIndicies.push_back(cnt - 2);
		mIndicies.push_back(cnt - 1);
	}

	mName = "GroundGrid";
	return true;
}
bool jMesh::LoadAxis(int _len)
{
	float arrowLen = 0.5f;

	{ //Axis X
		Vector4f color(1.0f, 0.0f, 0.0f, 1.0f);
		int baseCnt = mVerticies.size();
		VertexFormat vert;
		vert.color = color;
		vert.position = Vector3f(0.0f, 0.0f, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(_len, 0.0f, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(_len - arrowLen, arrowLen, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(_len - arrowLen, -arrowLen, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(_len - arrowLen, 0.0f, arrowLen);
		mVerticies.push_back(vert);
		vert.position = Vector3f(_len - arrowLen, 0.0f, -arrowLen);
		mVerticies.push_back(vert);

		mIndicies.push_back(baseCnt);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 2);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 3);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 4);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 5);
	}

	{ //Axis Y
		Vector4f color(0.0f, 1.0f, 0.0f, 1.0f);
		int baseCnt = mVerticies.size();
		VertexFormat vert;
		vert.color = color;
		vert.position = Vector3f(0.0f, 0.0f, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(0.0f, _len, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(0.0f, _len - arrowLen, arrowLen);
		mVerticies.push_back(vert);
		vert.position = Vector3f(0.0f, _len - arrowLen, -arrowLen);
		mVerticies.push_back(vert);
		vert.position = Vector3f(arrowLen, _len - arrowLen, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(-arrowLen, _len - arrowLen, 0.0f);
		mVerticies.push_back(vert);

		mIndicies.push_back(baseCnt);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 2);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 3);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 4);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 5);
	}

	{ //Axis Z
		Vector4f color(0.0f, 0.0f, 1.0f, 1.0f);
		int baseCnt = mVerticies.size();
		VertexFormat vert;
		vert.color = color;
		vert.position = Vector3f(0.0f, 0.0f, 0.0f);
		mVerticies.push_back(vert);
		vert.position = Vector3f(0.0f, 0.0f, _len);
		mVerticies.push_back(vert);
		vert.position = Vector3f(0.0f, arrowLen, _len - arrowLen);
		mVerticies.push_back(vert);
		vert.position = Vector3f(0.0f, -arrowLen, _len - arrowLen);
		mVerticies.push_back(vert);
		vert.position = Vector3f(arrowLen, 0.0f, _len - arrowLen);
		mVerticies.push_back(vert);
		vert.position = Vector3f(-arrowLen, 0.0f, _len - arrowLen);
		mVerticies.push_back(vert);

		mIndicies.push_back(baseCnt);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 2);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 3);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 4);
		mIndicies.push_back(baseCnt + 1);
		mIndicies.push_back(baseCnt + 5);
	}


	mName = "Axis";

	return true;
}