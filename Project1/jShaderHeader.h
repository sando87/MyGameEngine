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
struct VertexFormatPTC
{
	Vector3f p;
	Vector2f t;
	Vector4f c;
};
struct VertexFormatPT
{
	Vector3f p;
	Vector2f t;
};
struct VertexFormatPTI
{
	Vector3f p;
	Vector2f t;
	Vector4n i;
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
struct Billboard
{
	Vector3f transform; //xyz:position
	float size;
	float texIndex;
	float refDiscard;
	Vector2f reserve;
	Vector4f color; //xyz:rga, w:alpha
	Billboard()
	{
		transform = Vector3f(0, 0, 0);
		size = 1;
		texIndex = 0;
		refDiscard = 0;
		reserve = Vector2f(0, 0);
		color = Vector4f(1, 1, 1, 1);
	}
};
struct ShaderBufferBasic
{
	Matrix4f worldMat;
	Matrix4f viewMat;
	Matrix4f projectionMat;
	Vector4f matAmbient;
	Vector4f matDiffuse;
	Vector4f matSpecular;
	Vector4f matShininess;
	Vector4f lightPosition;
	Vector4f lightDirection;
	Vector4f lightColor;
	Vector4f lightReserve;
	Vector4f uvInfo;
	Vector4f texels[12];
	ShaderBufferBasic() { 
		memset(this, 0x00, sizeof(ShaderBufferBasic));
		worldMat.identity();
		viewMat.identity();
		projectionMat.identity();
		matDiffuse = Vector4f(1, 1, 1, 1);
		lightDirection = Vector4f(-1, -1, -1, 0);
	}
};
struct ShaderBufferSkin
{
	Matrix4f bones[45];
	ShaderBufferSkin() 
	{
		for (int i = 0; i < 45; ++i)
			bones[i].identity();
	}
};
struct ShaderBufferBillboards
{
	Matrix4f billboardMat;
	Billboard boards[45];
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
	ShaderParamsSkin()
	{
		memset(this, 0x00, sizeof(ShaderParamsSkin));
		for (int i = 0; i < 45; ++i)
			bones[i].identity();
		material.diffuse = Vector4f(1, 1, 1, 1);
		light.direction = Vector4f(-1, -1, -1, 0);
	}
};
struct ShaderParamsDefault //should be 16byte aligned
{
	ShaderBufferMaterial material;
	ShaderBufferLight light;
	ShaderParamsDefault()
	{
		material.diffuse = Vector4f(1, 1, 1, 1);
		light.direction = Vector4f(-1, -1, -1, 0);
	}
};
struct ShaderParamsTerrain //should be 16byte aligned
{
	Vector4f vectors[12];
	ShaderParamsTerrain() { memset(this, 0x00, sizeof(ShaderParamsTerrain)); }
};

#pragma pack(pop)