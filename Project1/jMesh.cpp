#include "jMesh.h"
#include "jLoader.h"
#include "jCaches.h"

jMesh::jMesh(string _fullname)
{
	Reset();
	SetFullname(_fullname);
}


jMesh::~jMesh()
{
	Reset();
}

void jMesh::OnLoad()
{
	string fullname = GetFullname();
	if (fullname.length() <= 0)
		return;

	jMesh * ret = (jMesh *)jCaches::CacheClass(fullname, [](string _fullname) {
		jMesh * mesh = new jMesh();
		mesh->SetFullname(_fullname);
		mesh->LoadFile();
		return mesh;
	});

	if (ret != nullptr)
	{
		mPrimitive = ret->mPrimitive;
		mVerticies = ret->mVerticies;
		mIndicies = ret->mIndicies;
		mBox = ret->mBox;
	}
}
void jMesh::UpdateBox()
{
	Vector3 minPos(1000, 1000, 1000);
	Vector3 maxPos(-1000, -1000, -1000);
	for (VertexFormat& vert : mVerticies)
	{
		minPos.x = min((double)vert.position.x, minPos.x);
		minPos.y = min((double)vert.position.y, minPos.y);
		minPos.z = min((double)vert.position.z, minPos.z);
		maxPos.x = max((double)vert.position.x, maxPos.x);
		maxPos.y = max((double)vert.position.y, maxPos.y);
		maxPos.z = max((double)vert.position.z, maxPos.z);
	}
	mBox = jRect3D(minPos, maxPos - minPos);
}
bool jMesh::LoadFile()
{
	string fullname = GetFullname();
	_exceptif(false == jUtils::ExistFile(fullname), _echoS(fullname); return false;);

	jLoader data;
	if (jUtils::GetFileExtension(fullname) == "obj")
	{
		data.LoadObjFile(fullname);
	}
	else if (jUtils::GetFileExtension(fullname) == "DAE")
	{
		data.LoadDaeFile(fullname);
	}
	else if (jUtils::GetFileExtension(fullname) == "dump")
	{
		chars buf = jUtils::LoadFile2(fullname);
		_warnif(buf->size() % sizeof(VertexFormat) != 0);
		int cnt = buf->size() / sizeof(VertexFormat);
		mVerticies.resize(cnt);
		memcpy(&mVerticies[0], &buf[0], buf->size());
		mPrimitive = PrimitiveMode::TriangleList;
		UpdateBox();
		return true;
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
		vertex.weights = info.Weights.size() > 0 ? info.Weights[vertIdx.x] : Vector4f(1, 0, 0, 0);

		mVerticies.push_back(vertex);

		//if (mVerticies.size() % 3 == 2)
		//{
		//	int startIdx = mVerticies.size();
		//	mIndicies.push_back(startIdx);
		//	mIndicies.push_back(startIdx - 2);
		//	mIndicies.push_back(startIdx - 1);
		//}
	}

	mPrimitive = PrimitiveMode::TriangleList;
	UpdateBox();
	return true;
}
bool jMesh::LoadCube(int size)
{
	double halfSize = size * 0.5;
	mVerticies.resize(8);
	mVerticies[0].position = Vector3(-halfSize,	halfSize,	-halfSize);
	mVerticies[1].position = Vector3(halfSize,	halfSize,	-halfSize);
	mVerticies[2].position = Vector3(-halfSize,	halfSize,	halfSize);
	mVerticies[3].position = Vector3(halfSize,	halfSize,	halfSize);
	mVerticies[4].position = Vector3(-halfSize,	-halfSize,	-halfSize);
	mVerticies[5].position = Vector3(halfSize,	-halfSize,	-halfSize);
	mVerticies[6].position = Vector3(-halfSize,	-halfSize,	halfSize);
	mVerticies[7].position = Vector3(halfSize,	-halfSize,	halfSize);

	mVerticies[0].color = Vector4f(1, 0, 0, 1);
	mVerticies[1].color = Vector4f(0, 1, 0, 1);
	mVerticies[2].color = Vector4f(0, 0, 1, 1);
	mVerticies[3].color = Vector4f(1, 1, 0, 1);
	mVerticies[4].color = Vector4f(1, 0, 1, 1);
	mVerticies[5].color = Vector4f(0, 1, 1, 1);
	mVerticies[6].color = Vector4f(0, 0, 0, 1);
	mVerticies[7].color = Vector4f(1, 1, 1, 1);

	mIndicies.resize(6 * 2 * 3);

	int idx = 0;
	mIndicies[idx++] = 0;	mIndicies[idx++] = 2;	mIndicies[idx++] = 1; //front
	mIndicies[idx++] = 1;	mIndicies[idx++] = 2;	mIndicies[idx++] = 3;
	mIndicies[idx++] = 4;	mIndicies[idx++] = 5;	mIndicies[idx++] = 6; //back
	mIndicies[idx++] = 4;	mIndicies[idx++] = 6;	mIndicies[idx++] = 5;
	mIndicies[idx++] = 1;	mIndicies[idx++] = 7;	mIndicies[idx++] = 5; //right
	mIndicies[idx++] = 1;	mIndicies[idx++] = 3;	mIndicies[idx++] = 7;
	mIndicies[idx++] = 0;	mIndicies[idx++] = 6;	mIndicies[idx++] = 2; //left
	mIndicies[idx++] = 0;	mIndicies[idx++] = 4;	mIndicies[idx++] = 6;
	mIndicies[idx++] = 7;	mIndicies[idx++] = 2;	mIndicies[idx++] = 6; //top
	mIndicies[idx++] = 7;	mIndicies[idx++] = 3;	mIndicies[idx++] = 2;
	mIndicies[idx++] = 5;	mIndicies[idx++] = 4;	mIndicies[idx++] = 0; //bottom
	mIndicies[idx++] = 5;	mIndicies[idx++] = 0;	mIndicies[idx++] = 1;

	mPrimitive = PrimitiveMode::TriangleList;
	SetFullname("Cube" + jUtils::ToString(size));
	UpdateBox();
	SetLoaded(true);
	return true;
}
bool jMesh::LoadCubeOutline(int size)
{
	double halfSize = size * 0.5;
	mVerticies.resize(8);
	mVerticies[0].position = Vector3(-halfSize,	halfSize,	-halfSize);
	mVerticies[1].position = Vector3(halfSize,	halfSize,	-halfSize);
	mVerticies[2].position = Vector3(-halfSize,	halfSize,	halfSize);
	mVerticies[3].position = Vector3(halfSize,	halfSize,	halfSize);
	mVerticies[4].position = Vector3(-halfSize,	-halfSize,	-halfSize);
	mVerticies[5].position = Vector3(halfSize,	-halfSize,	-halfSize);
	mVerticies[6].position = Vector3(-halfSize,	-halfSize,	halfSize);
	mVerticies[7].position = Vector3(halfSize,	-halfSize,	halfSize);

	mVerticies[0].color = Vector4f(0, 1, 0, 1);
	mVerticies[1].color = Vector4f(0, 1, 0, 1);
	mVerticies[2].color = Vector4f(0, 1, 0, 1);
	mVerticies[3].color = Vector4f(0, 1, 0, 1);
	mVerticies[4].color = Vector4f(0, 1, 0, 1);
	mVerticies[5].color = Vector4f(0, 1, 0, 1);
	mVerticies[6].color = Vector4f(0, 1, 0, 1);
	mVerticies[7].color = Vector4f(0, 1, 0, 1);

	mIndicies.resize(24);

	int idx = 0;
	//back
	mIndicies[idx++] = 0; mIndicies[idx++] = 1;
	mIndicies[idx++] = 1; mIndicies[idx++] = 3;
	mIndicies[idx++] = 3; mIndicies[idx++] = 2;
	mIndicies[idx++] = 2; mIndicies[idx++] = 0;

	//front
	mIndicies[idx++] = 4; mIndicies[idx++] = 5;
	mIndicies[idx++] = 5; mIndicies[idx++] = 7;
	mIndicies[idx++] = 7; mIndicies[idx++] = 6;
	mIndicies[idx++] = 6; mIndicies[idx++] = 4;

	//side
	mIndicies[idx++] = 0; mIndicies[idx++] = 4;
	mIndicies[idx++] = 1; mIndicies[idx++] = 5;
	mIndicies[idx++] = 2; mIndicies[idx++] = 6;
	mIndicies[idx++] = 3; mIndicies[idx++] = 7;

	mPrimitive = PrimitiveMode::LineList;
	SetFullname("CubeOutline" + jUtils::ToString(size));
	UpdateBox();
	SetLoaded(true);
	return true;
}
bool jMesh::LoadRectangle(Vector2 size)
{
	mVerticies.resize(4);
	mVerticies[0].position = Vector3(-size.x * 0.5, 0, -size.y * 0.5);
	mVerticies[1].position = Vector3(size.x * 0.5, 0, -size.y * 0.5);
	mVerticies[2].position = Vector3(-size.x * 0.5, 0, size.y * 0.5);
	mVerticies[3].position = Vector3(size.x * 0.5, 0, size.y * 0.5);

	mVerticies[0].texel = Vector2f(0, 1);
	mVerticies[1].texel = Vector2f(1, 1);
	mVerticies[2].texel = Vector2f(0, 0);
	mVerticies[3].texel = Vector2f(1, 0);

	mPrimitive = PrimitiveMode::TriangleStrip;
	SetFullname("Rectangle" + jUtils::ToString(size.length()));
	SetLoaded(true);
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

	mPrimitive = PrimitiveMode::LineList;
	SetFullname("Grid");
	UpdateBox();
	SetLoaded(true);
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

	mPrimitive = PrimitiveMode::LineList;
	SetFullname("Grid");
	UpdateBox();
	SetLoaded(true);

	return true;
}

bool jMesh::LoadVerticies(vector<VertexFormat>& verticies, vector<u32>& indicies, string name)
{
	mIndicies = indicies;
	mVerticies = verticies;

	mPrimitive = mIndicies.empty() ? PrimitiveMode::TriangleStrip : PrimitiveMode::TriangleList;
	SetFullname(name);
	UpdateBox();
	SetLoaded(true);
	return true;
}

void jMesh::Reset()
{
	SetFullname("");
	mBox = jRect3D();
	mVerticies.clear();
	mIndicies.clear();
	mPrimitive = PrimitiveMode::None;
}
