#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jLoader.h"

struct VertexFormat
{
	Vector3f position;
	Vector2f texel;
	Vector3f normal;
	Vector4f color;
	Vector4n boneIndexs;
	Vector4f weights;
};

class jMesh :	public jComponent
{
public:
	jMesh(string _name = "");
	virtual ~jMesh();

	vector<VertexFormat>& GetVerticies() { return mVerticies; }
	vector<u32>& GetIndicies() { return mIndicies; }

	bool Load(string _name);
	bool LoadGrid(int _x, int _y, int _w, int _h, int _step);
	bool LoadAxis(int _len);

private:
	vector<VertexFormat> mVerticies;
	vector<u32> mIndicies;

	Property_Getter(string, Name)
};

