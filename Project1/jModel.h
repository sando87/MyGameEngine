#ifndef __JMODEL_H__
#define __JMODEL_H__

#include "header.h"
#include "Vectors.h"

class jModel
{
private:
	struct VertexType_Weight
	{
		Vector3f p;
		Vector2f t;
		Vector3f n;
		Vector4n index;
		Vector4f weight;
	};
	struct VertexType_Sprite
	{
		Vector3f p;
		Vector2f t;
	};

public:
	jModel();
	~jModel();

	bool Load(string _name);
	bool LoadPointList(vector<Vector3>& vec, float size);
	bool LoadSprite(Vector3 _point, Vector2f _uvStep);
	void Release();

	int GetVertexTypeSize() { return m_sizeVertex; }
	int GetIndexCount() { return m_indexCount; }
	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }

private:
	//void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	int m_sizeVertex;
};

#endif