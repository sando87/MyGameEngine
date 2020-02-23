////////////////////////////////////////////////////////////////////////////////
// Filename: ui.vs
////////////////////////////////////////////////////////////////////////////////


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


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType jVS(VertexInputType input)
{
    PixelInputType output;

	input.position.w = 1.0f;
	
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	
    output.tex = input.tex;
	output.color = input.color;
    
    return output;
}