#pragma once

#pragma pack(push, 1)
#include "Vectors.h"
#include "Matrices.h"


struct VertexFormatPTN
{
	Vector3f p;
	Vector2f t;
	Vector3f n;
};
struct VertexFormatPT
{
	Vector3f p;
	Vector2f t;
};
struct VertexFormatPTNIW
{
	Vector3f p;
	Vector2f t;
	Vector3f n;
	Vector4n i;
	Vector4f w;
};
struct VertexFormatPC
{
	Vector3f p;
	Vector4f c;
};


struct ShaderBufferWVP //should be 16byte aligned
{
	Matrix4f world;
	Matrix4f view;
	Matrix4f projection;
};
struct ShaderBufferMatrixs //should be 16byte aligned
{
	Matrix4f world;
	Matrix4f view;
	Matrix4f projection;
	Matrix4f bones[45];
};
struct ShaderBufferMaterial //should be 16byte aligned
{
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	Vector4f shininess;
};
struct ShaderBufferLight //should be 16byte aligned
{
	Vector4f position;
	Vector4f direction;
	Vector4f color;
	Vector4f reserve;
};

struct ShaderParamsSprite //should be 16byte aligned
{
	Vector2n ImageIDX;
	Vector2f step;
	Vector4f ambient;
};
struct ShaderParamsSkin //should be 16byte aligned
{
	Matrix4f bones[45];
	ShaderBufferMaterial material;
	ShaderBufferLight light;
};
struct ShaderParamsDefault //should be 16byte aligned
{
	ShaderBufferMaterial material;
	ShaderBufferLight light;
};
struct ShaderParamsTerrain //should be 16byte aligned
{
	Vector4f vectors[12];
};

#pragma pack(pop)