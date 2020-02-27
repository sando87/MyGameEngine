////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////

struct Billboard
{
    matrix worldMat;
    float2 texelIndex;
    float  refDiscard;
    float  reserve;
};

/////////////
// GLOBALS //
/////////////
cbuffer BasicBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
    float4 g_ambient;
    float4 g_diffuse;
	float4 g_specular;
	float4 g_shininess;
    float4 g_position;
    float4 g_direction;
	float4 g_color;
	float4 g_reserve;
    float4 g_uvInfo;
	float4 texels[12];
};
cbuffer BillboardBuffer : register(b1)
{
	Billboard boards[45];
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    uint4   index : INDEX;
    float4 weight : WEIGHT;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    uint4  index : INDEX;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType jVS(VertexInputType input)
{
    PixelInputType output;
	uint idx;
	float2 texTrans;
    
    input.position.w = 1.0f;
	idx = input.index[0];

    output.position = mul(input.position, boards[idx].worldMat);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	float stepU = g_uvInfo.z;
	float stepV = g_uvInfo.w;
	int idxU = boards[idx].texelIndex.x;
	int idxV = boards[idx].texelIndex.y;
	texTrans.x = (stepU * idxU) + input.tex.x;
	texTrans.y = (stepV * idxV) + input.tex.y;
    output.tex = texTrans;
	output.index = input.index;
    
    return output;
}