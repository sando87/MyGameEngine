#pragma once
#include "jComponent.h"
#include "junks.h"

struct VertexFormat
{
	Vector3f position;
	Vector2f texel;
	Vector3f normal;
	Vector4f color;
	Vector4n boneIndexs;
	Vector4f weights;
};

enum PrimitiveMode
{
	None,
	LineList,
	TriangleList,
	TriangleStrip,
};

class jMesh :	public jComponent
{
public:
	jMesh(string _fullname = "");
	virtual ~jMesh();

	virtual void OnLoad();

	bool LoadFile();
	bool LoadCube(int size);
	bool LoadRectangle(Vector2 center, Vector2 size);
	bool LoadGrid(int _x, int _y, int _w, int _h, int _step);
	bool LoadAxis(int _len);
	void Reset();

	vector<VertexFormat>& GetVerticies() { return mVerticies; }
	vector<u32>& GetIndicies() { return mIndicies; }
	vector<char>& GetStream() { return mStream; }

protected:

	vector<VertexFormat> mVerticies;
	vector<u32> mIndicies;
	vector<char> mStream;

	Property_Getter(PrimitiveMode, Primitive, PrimitiveMode::None)
};

