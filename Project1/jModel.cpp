#include "jModel.h"
#include "jGlobalStruct.h"
#include "jRenderer.h"
#include "jLoader.h"
#include "jUtils.h"
#include "jGlobalStruct.h"

jModel::jModel()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_sizeVertex = 0;
	m_sizeIndex = 0;
}

jModel::~jModel()
{
	Release();
}

bool jModel::Load(string _name)
{
	jLoader loader;
	if (jUtils::GetFileExtension(_name) == "obj")
	{
		loader.LoadObjFile(_name);
	}
	else if (jUtils::GetFileExtension(_name) == "DAE")
	{
		loader.LoadDaeFile(_name);
	}
	
	vector<VertexType_Weight> vertices;
	vector<Vector3n> faces;

	int idxVertex = 0;
	int cnt = loader.mFaceInfo.size();
	for (int i = 0; i < cnt; ++i)
	{
		vector<Vector3n>& faceInfo = loader.mFaceInfo[i];
		int faceCnt = faceInfo.size();
		for (int j = 0; j < faceCnt; ++j)
		{
			VertexType_Weight vertex;
			vertex.p = loader.mPos[faceInfo[j].x];
			vertex.t = loader.mUV[faceInfo[j].y];
			vertex.n = loader.mNormal[faceInfo[j].z];
			vector<BoneWeight>& vecWeights = loader.mWeightPos[faceInfo[j].x].weights;
			if (vecWeights.size() == 0)
			{
				vertex.index = Vector4n(0,0,0,0);
				vertex.weight = Vector4f(1,0,0,0);
			}
			else
			{
				for (int k = 0; k < 4; ++k)
				{
					if (k < (int)vecWeights.size())
					{
						vertex.index[k] = vecWeights[k].boneIndex;
						vertex.weight[k] = vecWeights[k].weight;
					}
					else
					{
						vertex.index[k] = 0;
						vertex.weight[k] = 0.0f;
					}
				}
			}
			
			vertices.push_back(vertex);
		}

		faces.push_back(Vector3n(idxVertex, idxVertex+2, idxVertex+1));
		if (faceCnt == 4)
			faces.push_back(Vector3n(idxVertex, idxVertex+2, idxVertex+3));
		idxVertex += faceCnt;
	}

	m_vertexCount = vertices.size();
	m_indexCount = faces.size() * 3;
	m_sizeVertex = sizeof(VertexType_Weight);

	// 정적 정점 버퍼의 구조체를 설정합니다.
	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType_Weight) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &faces[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	return true;
}
bool jModel::LoadSprite(Vector3 _point, Vector2f _uvStep)
{
	vector<VertexType_Sprite> vertices;
	vector<Vector3n> faces;

	float size = 1.0f;
	Vector3f a(-size, size, 0);
	Vector3f b(-size, -size, 0);
	Vector3f c(size, size, 0);
	Vector3f d(size, -size, 0);

	int base = 0;
	Vector3f pt = _point;
	VertexType_Sprite vert;
	vert.p = (pt + a);
	vert.t = Vector2f(0.0f, 0.0f);
	vertices.push_back(vert);
	vert.p = (pt + b);
	vert.t = Vector2f(0.0f, _uvStep.y);
	vertices.push_back(vert);
	vert.p = (pt + c);
	vert.t = Vector2f(_uvStep.x, 0.0f);
	vertices.push_back(vert);
	vert.p = (pt + d);
	vert.t = Vector2f(_uvStep.x, _uvStep.y);
	vertices.push_back(vert);
	faces.push_back(Vector3n(base + 0, base + 2, base + 1)); //CW (CCW : X)
	faces.push_back(Vector3n(base + 1, base + 2, base + 3)); //CW

	m_vertexCount = vertices.size();
	m_indexCount = faces.size() * 3;
	m_sizeVertex = sizeof(VertexType_Sprite);

	// 정적 정점 버퍼의 구조체를 설정합니다.
	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType_Sprite) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &faces[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	return true;

}
bool jModel::LoadPointList(vector<Vector3>& vec, float size)
{
	vector<VertexType_Weight> vertices;
	vector<Vector3n> faces;

	Vector3f a(-size, -size, -size);
	Vector3f b(-size, -size, size);
	Vector3f c(size, -size, 0);
	Vector3f d(0, size, 0);

	for (int i = 0; i < vec.size(); ++i)
	{
		int base = vertices.size();
		Vector3f pt = vec[i];
		VertexType_Weight vert;
		vert.weight[0] = 1.0f;
		vert.n = Vector3f(1, 1, 1);
		vert.t = Vector2f(0.5f, 0.5f);
		vert.p = (pt + a);
		vertices.push_back(vert);
		vert.p = (pt + b);
		vertices.push_back(vert);
		vert.p = (pt + c);
		vertices.push_back(vert);
		vert.p = (pt + d);
		vertices.push_back(vert);
		faces.push_back(Vector3n(base + 0, base + 1, base + 2));
		faces.push_back(Vector3n(base + 1, base + 2, base + 3));
		faces.push_back(Vector3n(base + 2, base + 3, base + 0));
		faces.push_back(Vector3n(base + 3, base + 0, base + 1));

	}

	m_vertexCount = vertices.size();
	m_indexCount = faces.size() * 3;
	m_sizeVertex = sizeof(VertexType_Weight);

	// 정적 정점 버퍼의 구조체를 설정합니다.
	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType_Weight) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &faces[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	return true;
}
bool jModel::LoadGrid(int _x, int _y, int _w, int _h, int _step)
{
	vector<VertexType_Color> vertices;
	vector<Vector2n> lines;

	int left = _x;
	int top = _y;
	int right = _x + _w;
	int bottom = _y - _h;
	for (int i = left; i <= right; i += _step)
	{
		if (i == 0)
			continue;

		VertexType_Color vert;
		vert.c = Vector4f(1.0f,1.0f,1.0f,1.0f);
		vert.p = Vector3f(i, top, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(i, bottom, 0.0f);
		vertices.push_back(vert);

		int cnt = vertices.size();
		lines.push_back(Vector2n(cnt-2, cnt-1));
	}

	for (int i = top; i >= bottom; i -= _step)
	{
		if (i == 0)
			continue;

		VertexType_Color vert;
		vert.c = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
		vert.p = Vector3f(left, i, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(right, i, 0.0f);
		vertices.push_back(vert);

		int cnt = vertices.size();
		lines.push_back(Vector2n(cnt - 2, cnt - 1));
	}


	m_vertexCount = vertices.size();
	m_indexCount = lines.size() * 2;
	m_sizeVertex = sizeof(VertexType_Color);

	// 정적 정점 버퍼의 구조체를 설정합니다.
	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType_Color) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &lines[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	return true;
}
bool jModel::LoadAxis(int _len)
{
	float arrowLen = 0.5f;
	vector<VertexType_Color> vertices;
	vector<Vector2n> lines;

	{ //Axis X
		Vector4f color(1.0f, 0.0f, 0.0f, 1.0f);
		int baseCnt = vertices.size();
		VertexType_Color vert;
		vert.c = color;
		vert.p = Vector3f(0.0f, 0.0f, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(_len, 0.0f, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(_len - arrowLen, arrowLen, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(_len - arrowLen, -arrowLen, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(_len - arrowLen, 0.0f, arrowLen);
		vertices.push_back(vert);
		vert.p = Vector3f(_len - arrowLen, 0.0f, -arrowLen);
		vertices.push_back(vert);

		lines.push_back(Vector2n(baseCnt, baseCnt + 1));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 2));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 3));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 4));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 5));
	}

	{ //Axis Y
		Vector4f color(0.0f, 1.0f, 0.0f, 1.0f);
		int baseCnt = vertices.size();
		VertexType_Color vert;
		vert.c = color;
		vert.p = Vector3f(0.0f, 0.0f, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(0.0f, _len, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(0.0f, _len - arrowLen, arrowLen);
		vertices.push_back(vert);
		vert.p = Vector3f(0.0f, _len - arrowLen, -arrowLen);
		vertices.push_back(vert);
		vert.p = Vector3f(arrowLen, _len - arrowLen, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(-arrowLen, _len - arrowLen, 0.0f);
		vertices.push_back(vert);

		lines.push_back(Vector2n(baseCnt, baseCnt + 1));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 2));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 3));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 4));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 5));
	}

	{ //Axis Z
		Vector4f color(0.0f, 0.0f, 1.0f, 1.0f);
		int baseCnt = vertices.size();
		VertexType_Color vert;
		vert.c = color;
		vert.p = Vector3f(0.0f, 0.0f, 0.0f);
		vertices.push_back(vert);
		vert.p = Vector3f(0.0f, 0.0f, _len);
		vertices.push_back(vert);
		vert.p = Vector3f(0.0f, arrowLen, _len - arrowLen);
		vertices.push_back(vert);
		vert.p = Vector3f(0.0f, -arrowLen, _len - arrowLen);
		vertices.push_back(vert);
		vert.p = Vector3f(arrowLen, 0.0f, _len - arrowLen);
		vertices.push_back(vert);
		vert.p = Vector3f(-arrowLen, 0.0f, _len - arrowLen);
		vertices.push_back(vert);

		lines.push_back(Vector2n(baseCnt, baseCnt + 1));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 2));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 3));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 4));
		lines.push_back(Vector2n(baseCnt + 1, baseCnt + 5));
	}


	m_vertexCount = vertices.size();
	m_indexCount = lines.size() * 2;
	m_sizeVertex = sizeof(VertexType_Color);

	// 정적 정점 버퍼의 구조체를 설정합니다.
	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType_Color) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &lines[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	return true;
}

bool jModel::LoadDiablo_FromRes(MyResBase* _res, MyResBase* _layout)
{
	if (_res == NULL)
		return false;

	if (_res->type != MYRES_TYPE_CreateBuffer)
		return false;

	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	MyRes_CreateBuffer *pData = (MyRes_CreateBuffer*)_res;
	MyRes_CreateLayout *pLayout = (MyRes_CreateLayout*)_layout;
	if (pData->desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER)
	{
		unsigned int stride = pLayout->GetStride(0);
		unsigned int textureOff = pLayout->GetTextureOffset(0);
		m_sizeVertex = sizeof(VertexType_Texture);
		m_vertexCount = pData->desc.ByteWidth / stride;

		vector<VertexType_Texture> verticies;
		char* tmp = pData->data;
		unsigned char* tex = nullptr;
		Matrix4 matTex;
		memset(&matTex, 0x00, sizeof(Matrix4));
		matTex[0] = 5.0f;
		matTex[5] = 5.0f;
		for (int i = 0; i < m_vertexCount; ++i)
		{
			VertexType_Texture vert;
			memcpy(&vert.p, tmp, sizeof(Vector3f));
			tex = (unsigned char*)(tmp + textureOff);

			Vector4f tmp1;
			tmp1.x = (unsigned char)tex[1];
			tmp1.y = (unsigned char)tex[0];
			tmp1.z = (unsigned char)tex[3];
			tmp1.w = (unsigned char)tex[2];

			Vector4f tmp2;
			tmp2.x = tmp1.y * 0.003906f + tmp1.x;
			tmp2.y = tmp1.w * 0.003906f + tmp1.z;
			tmp2.x = tmp2.x * 0.5f - 64.0f;
			tmp2.y = tmp2.y * 0.5f - 64.0f;
			tmp2.z = 1.0f;

			Vector4f tmp3;
			tmp3.x = tmp2.x * matTex[0] + tmp2.y * matTex[1] + tmp2.z * matTex[3] + tmp2.x * matTex[0];
			tmp3.y = tmp2.x * matTex[4] + tmp2.y * matTex[5] + tmp2.z * matTex[7] + tmp2.x * matTex[4];
			tmp3.z = tmp2.x * matTex[8] + tmp2.y * matTex[9] + tmp2.z * matTex[11] + tmp2.x * matTex[8];
			tmp3.w = tmp2.x * matTex[12] + tmp2.y * matTex[13] + tmp2.z * matTex[15] + tmp2.x * matTex[12];

			vert.t.x = tmp3.x;
			vert.t.y = tmp3.y;

			tmp += stride;
			verticies.push_back(vert);
		}

		// 정적 정점 버퍼의 구조체를 설정합니다.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = m_sizeVertex * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = &verticies[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
			return false;
	}
	else if (pData->desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER)
	{
		m_sizeIndex = 2;
		m_indexCount = pData->desc.ByteWidth / m_sizeIndex;

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = pData->data;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&pData->desc, &indexData, &m_indexBuffer)))
		{
			return false;
		}
	}

	return true;
}

bool jModel::LoadDiablo_color()
{
	FILE *pFileVert = NULL, *pFileIndex = NULL;
	MyRes_CreateBuffer *pDataVert = NULL, *pDataIndex = NULL;
	int filesizeVert, filesizeIndex = 0;
	//fopen_s(&pFileVert, "D:\\temp_1\\vertexBuf_000001C8350369A0_0_0.dump", "rb"); 
	//fopen_s(&pFileIndex, "D:\\temp_1\\indexBuf_000001C835036160_0.dump", "rb");

	fopen_s(&pFileVert, "D:\\temp\\object1\\b_000001A2A689C520_4024_168.dump", "rb");
	fopen_s(&pFileIndex, "D:\\temp\\object1\\b_000001A2A689A6E0_740_96.dump", "rb");

	fseek(pFileVert, 0, SEEK_END);
	filesizeVert = ftell(pFileVert);
	fseek(pFileVert, 0, SEEK_SET);
	fseek(pFileIndex, 0, SEEK_END);
	filesizeIndex = ftell(pFileIndex);
	fseek(pFileIndex, 0, SEEK_SET);

	pDataVert = (MyRes_CreateBuffer*)malloc(filesizeVert);
	fread_s(pDataVert, filesizeVert, filesizeVert, 1, pFileVert);
	pDataIndex = (MyRes_CreateBuffer*)malloc(filesizeIndex);
	fread_s(pDataIndex, filesizeIndex, filesizeIndex, 1, pFileIndex);

	m_sizeVertex = sizeof(VertexType_Color);
	m_sizeIndex = 2;
	UINT vertexStride = 32;
	m_vertexCount = pDataVert->desc.ByteWidth / vertexStride;
	m_indexCount = pDataIndex->desc.ByteWidth / m_sizeIndex;
	m_indexCount = 312; //1695; 1833;

	vector<VertexType_Color> verticies;
	char* tmp = pDataVert->data;
	for (int i = 0; i < m_vertexCount; ++i)
	{
		VertexType_Color vert;
		memcpy(&vert.p, tmp, sizeof(Vector3f));
		vert.c = Vector4f(0, 1, 0, 1);
		tmp += vertexStride;

		verticies.push_back(vert);
	}

	//vector<unsigned short> indicies;
	//char *pIdicies = pDataIndex->data;
	//for (int i = 0; i < m_indexCount; ++i)
	//{
	//	unsigned short vert;
	//	memcpy(&vert, pIdicies, 2);
	//	pIdicies += 2;
	//	indicies.push_back(vert);
	//}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	ID3D11Device * device = jRenderer::GetInst().GetDevice();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = m_sizeVertex * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &verticies[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pDataIndex->data;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	if (FAILED(device->CreateBuffer(&pDataIndex->desc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	fclose(pFileVert);
	fclose(pFileIndex);
	if (pDataVert)
		free(pDataVert);
	if (pDataIndex)
		free(pDataIndex);

	return true;
}

bool jModel::LoadDiablo()
{
	{
		FILE *pFileVert = NULL, *pFileIndex = NULL, *pFileCV = NULL;
		MyBuffer *pDataVert = NULL, *pDataIndex = NULL, *pDataCV = NULL;
		int filesizeVert, filesizeIndex = 0, filesizeCV = 0;
		//fopen_s(&pFileVert, "D:\\temp_1\\vertexBuf_000001C8350369A0_0_0.dump", "rb"); 
		//fopen_s(&pFileIndex, "D:\\temp_1\\indexBuf_000001C835036160_0.dump", "rb");

		fopen_s(&pFileVert, "D:\\temp\\vertexBuf_1_0_0000023AB0DE0EE0.dump", "rb"); 
		fopen_s(&pFileIndex, "D:\\temp\\indexBuf_1_0_0000023AB0DE1460.dump", "rb");
		fopen_s(&pFileCV, "D:\\temp\\VSConstBuf_0000023A5B54C5E0.dump", "rb");

		fseek(pFileVert, 0, SEEK_END);
		filesizeVert = ftell(pFileVert);
		fseek(pFileVert, 0, SEEK_SET);
		fseek(pFileIndex, 0, SEEK_END);
		filesizeIndex = ftell(pFileIndex);
		fseek(pFileIndex, 0, SEEK_SET);
		fseek(pFileCV, 0, SEEK_END);
		filesizeCV = ftell(pFileCV);
		fseek(pFileCV, 0, SEEK_SET);

		pDataVert = (MyBuffer*)malloc(filesizeVert);
		fread_s(pDataVert, filesizeVert, filesizeVert, 1, pFileVert);
		pDataIndex = (MyBuffer*)malloc(filesizeIndex);
		fread_s(pDataIndex, filesizeIndex, filesizeIndex, 1, pFileIndex);
		pDataCV = (MyBuffer*)malloc(filesizeCV);
		fread_s(pDataCV, filesizeCV, filesizeCV, 1, pFileCV);
		CBMain* cbm = (CBMain*)pDataCV->data;

		m_sizeVertex = sizeof(VertexType_Weight);
		m_sizeIndex = 2;
		m_vertexCount = pDataVert->desc.ByteWidth / 32;
		m_indexCount = pDataIndex->desc.ByteWidth / m_sizeIndex;
		m_indexCount = 1833; //1695; 1833;

		vector<VertexType_Weight> verticies;
		VertexType_Diablo* tmp = (VertexType_Diablo*)pDataVert->data;
		for (int i = 0; i < m_vertexCount; ++i)
		{
			VertexType_Weight vert;
			vert.p = tmp[i].p;
			vert.n = Vector3f(0.0f, 0.0f, 1.0f);
			vert.weight.x = 1.0f;

			Vector4f tmp1;
			tmp1.x = (unsigned char)tmp[i].t1[1];
			tmp1.y = (unsigned char)tmp[i].t1[0];
			tmp1.z = (unsigned char)tmp[i].t1[3];
			tmp1.w = (unsigned char)tmp[i].t1[2];

			Vector4f tmp2;
			tmp2.x = tmp1.y * 0.003906f + tmp1.x;
			tmp2.y = tmp1.w * 0.003906f + tmp1.z;
			tmp2.x = tmp2.x * 0.5f - 64.0f;
			tmp2.y = tmp2.y * 0.5f - 64.0f;
			tmp2.z = 1.0f;

			Vector4f tmp3;
			tmp3.x = tmp2.x * cbm->matTex1[0] + tmp2.y * cbm->matTex1[1] + tmp2.z * cbm->matTex1[3] + tmp2.x * cbm->matTex1[0];
			tmp3.y = tmp2.x * cbm->matTex1[4] + tmp2.y * cbm->matTex1[5] + tmp2.z * cbm->matTex1[7] + tmp2.x * cbm->matTex1[4];
			tmp3.z = tmp2.x * cbm->matTex1[8] + tmp2.y * cbm->matTex1[9] + tmp2.z * cbm->matTex1[11] + tmp2.x * cbm->matTex1[8];
			tmp3.w = tmp2.x * cbm->matTex1[12] + tmp2.y * cbm->matTex1[13] + tmp2.z * cbm->matTex1[15] + tmp2.x * cbm->matTex1[12];
			
			vert.t.x = tmp3.x;
			vert.t.y = tmp3.y;
			
			verticies.push_back(vert);
		}

		//vector<unsigned short> indicies;
		//tmp = pDataIndex->data;
		//for (int i = 0; i < m_indexCount; ++i)
		//{
		//	unsigned short vert;
		//	memcpy(&vert, tmp, 2);
		//	tmp += 2;
		//	indicies.push_back(vert);
		//}

		// 정적 정점 버퍼의 구조체를 설정합니다.
		ID3D11Device * device = jRenderer::GetInst().GetDevice();
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = m_sizeVertex * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = &verticies[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
		{
			return false;
		}

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = pDataIndex->data;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&pDataIndex->desc, &indexData, &m_indexBuffer)))
		{
			return false;
		}

		fclose(pFileVert);
		fclose(pFileIndex);
		fclose(pFileCV);
		if (pDataVert)
			free(pDataVert);
		if (pDataIndex)
			free(pDataIndex);
		if (pDataCV)
			free(pDataCV);
	}
	
	{
		FILE *pFile = NULL;
		MyBuffer *pData = NULL;
		int filesize = 0;
		fopen_s(&pFile, "D:\\temp\\vertexBuf_0_1_0000023AC0D51860.dump", "rb");
	
		fseek(pFile, 0, SEEK_END);
		filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
	
		pData = (MyBuffer*)malloc(filesize);
		fread_s(pData, filesize, filesize, 1, pFile);
	
	
		ID3D11Device * device = jRenderer::GetInst().GetDevice();
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = pData->data;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&pData->desc, &vertexData, &m_vertexBuffer2)))
		{
			return false;
		}
	
		fclose(pFile);
		if (pData)
			free(pData);
	}

	return true;
}

void jModel::Release()
{
	m_vertexCount = 0;
	m_indexCount = 0;

	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼를 해제합니다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}
/*
void jModel::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형으로 설정합니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
*/