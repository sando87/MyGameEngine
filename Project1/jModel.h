#ifndef __JMODEL_H__
#define __JMODEL_H__

#include "header.h"
#include "Vectors.h"
class jParserD3;
struct VertexType_Texture;

class jModel
{
public:
	jModel();
	~jModel();

	bool Load(string _name);
	bool LoadPointList(vector<Vector3>& vec, float size);
	bool LoadGrid(int _x, int _y, int _w, int _h, int _step);
	bool LoadAxis(int _len);
	bool LoadSprite(Vector3 _point, Vector2f _uvStep);
	bool LoadSimpleRect(int _len);
	bool LoadHeightMap(string _filename, float _step, float _maxHeight);

	bool LoadDiablo_ForTextureShader(jParserD3 *_context);
	bool LoadDiablo_ForSkkinedShader(jParserD3 *_context);
	bool LoadDiablo_ForTerrain(jParserD3 *_context);

	void Release();

	int GetVertexTypeSize() { return m_sizeVertex; }
	int GetIndexCount() { return m_indexCount; }
	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
	bool IsIndiciesStrideTwo() { return m_sizeIndex == 2 ? true : false; }
	vector<VertexType_Texture>& GetVerticies() { return mVerticies; }

public:
	int mStartIndex;
	int mVertexOff;
	int mVertexOff_setting;
	int mPrimitiveTopology;

private:
	vector<VertexType_Texture> mVerticies;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	int m_sizeVertex;
	int m_sizeIndex;

};

#endif