
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
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType jVS(VertexInputType input)
{
    PixelInputType output;

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
    return output;
}