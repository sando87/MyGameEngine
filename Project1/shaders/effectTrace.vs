////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////

struct Billboard
{
    float4 transform; //xyz:position, w:size
    float4 info; //x:texIdx, y:refDiscard, z:rotate, w:reserve
    float4 color; //xyz:rga, w:alpha
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
    float4 g_uvInfo; //x:CountU, y:CountV, z:stepU, w:stepV
	float4 texels[12];
};
cbuffer BillboardBuffer : register(b1)
{
	matrix billboardMat;
	Billboard boards[45];
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    uint4  index : INDEX;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 tex : TEXCOORD0;
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType jVS(VertexInputType input)
{
    PixelInputType output;
    
	Billboard board = boards[input.index.x];
	float size = board.transform.w;
	float rotate = board.info.z;
	float4 inputVertex;
	
	input.position.xyz *= size;
	inputVertex.x = input.position.x * cos(rotate) - input.position.z * sin(rotate);
	inputVertex.z = input.position.x * sin(rotate) + input.position.z * cos(rotate);
	inputVertex.y = input.position.y;
	inputVertex.w = 1.0f;
    output.position = mul(inputVertex, billboardMat);
	output.position.xyz += board.transform.xyz;
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	float stepU = g_uvInfo.z;
	float stepV = g_uvInfo.w;
	int idxU = board.info.x % g_uvInfo.x;
	int idxV = board.info.x / g_uvInfo.x;
	output.tex.x = stepU * (idxU + input.tex.x);
	output.tex.y = stepV * (idxV + input.tex.y);
	output.tex.z = board.info.y;
	output.color = board.color;
    
    return output;
}