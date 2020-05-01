#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jRect3D.h"

struct VertexFormat
{
	Vector3f position;
	Vector2f texel;
	Vector3f normal;
	Vector4f color;
	Vector4n boneIndexs;
	Vector4f weights;
	VertexFormat()
	{
		position = Vector3f();
		texel = Vector2f();
		normal = Vector3f(1, 1, 1);
		color = Vector4f(1, 1, 1, 1);
		boneIndexs = Vector4n(0, 0, 0, 0);
		weights = Vector4f(1, 0, 0, 0);
	}
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

	bool LoadFile();
	bool LoadCube(int size);
	bool LoadCubeOutline(int size);
	bool LoadRectangle(Vector2 size);
	bool LoadGrid(int _x, int _y, int _w, int _h, int _step);
	bool LoadAxis(int _len);
	bool LoadVerticies(vector<VertexFormat>& verticies, vector<u32>& indicies, string name);
	void Reset();

	vector<VertexFormat>& GetVerticies() { return mVerticies; }
	vector<u32>& GetIndicies() { return mIndicies; }
	jRect3D GetBox() { return mBox; }

protected:
	virtual void OnLoad();

	vector<VertexFormat> mVerticies;
	vector<u32> mIndicies;
	jRect3D mBox;

	void UpdateBox();

	Property_Getter(PrimitiveMode, Primitive, PrimitiveMode::None)
};

