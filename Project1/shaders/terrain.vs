////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.vs
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
    float4 tex0 : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
    float2 tex2 : TEXCOORD2;
    float2 tex3 : TEXCOORD3;
    float2 tex4 : TEXCOORD4;
    float2 tex5 : TEXCOORD5;
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
	input.tex0.z = 1.0f;
	input.tex0.w = 0;
    output.tex0.x = dot(input.tex0, texels[0]);
	output.tex0.y = dot(input.tex0, texels[1]);
    output.tex1.x = dot(input.tex0, texels[2]);
	output.tex1.y = dot(input.tex0, texels[3]);
    output.tex2.x = dot(input.tex0, texels[4]);
	output.tex2.y = dot(input.tex0, texels[5]);
    output.tex3.x = dot(input.tex0, texels[6]);
	output.tex3.y = dot(input.tex0, texels[7]);
    output.tex4.x = dot(input.tex0, texels[8]);
	output.tex4.y = dot(input.tex0, texels[9]);
    output.tex5.x = dot(input.tex0, texels[10]);
	output.tex5.y = dot(input.tex0, texels[11]);    
    
    return output;
}