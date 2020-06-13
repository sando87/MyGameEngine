////////////////////////////////////////////////////////////////////////////////
// Filename: iceBolt.vs
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
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType jVS(VertexInputType input)
{
    PixelInputType output;

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;
	float3 camWorldPos = float3(g_reserve.x, g_reserve.y, g_reserve.z);

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
	float3 dirToCam = camWorldPos - float3(output.position.x, output.position.y, output.position.z);
	dirToCam = normalize(dirToCam);
	
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
	// 반사광 계산
	float3 lightDir = float3(g_direction.x, g_direction.y, g_direction.z);
    float3 worldNormal = mul(input.normal, worldMatrix);
	worldNormal = normalize(worldNormal);
	float dotReflect = dot(worldNormal, -lightDir);
	float3 reflectVec = lightDir + (2 * dotReflect * worldNormal);
	float dotSpecular = 1 - length(cross(dirToCam, reflectVec));
    output.normal = float3(g_specular.x, g_specular.y, g_specular.z) * dotSpecular;
    
    return output;
}