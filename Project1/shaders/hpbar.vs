struct HPInfo
{
    float4 worldPos; //xyz:position, w:1.0
    float4 info; //xy: size, w:current HP rate
    float4 color; //xyz:rga, w:alpha
};

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
	HPInfo bars[45];
};


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
    
	HPInfo bar = bars[input.index.x];
	float4 pos = bar.worldPos;
	pos.w = 1.0;
	
    pos = mul(pos, worldMatrix);
    pos = mul(pos, viewMatrix);
    pos = mul(pos, projectionMatrix);
	float xpos = pos.x / pos.w;
	float ypos = pos.y / pos.w;
	
	output.position.x = (input.position.x * bar.info.x) + xpos;
	output.position.y = (input.position.y * bar.info.y) + ypos;
	output.position.z = 0; //nearest Z of camera
	output.position.w = 1;
    
	output.tex.x = input.tex.x;
	output.tex.y = input.tex.y;
	output.tex.z = bar.info.w; //transfer to ps ref Info
	output.color = bar.color;
    
    return output;
}