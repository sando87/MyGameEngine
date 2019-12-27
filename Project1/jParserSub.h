#pragma once

#include "junks.h"
#include "jDirectXheader.h"

#pragma pack(push, 1)

#define SIZE_CBMAIN 2440
#define SIZE_CBBONES 2216
#define SIZE_CBLIGHTS 6248

#define MYRES_TYPE_CreateBuffer	'b'
#define MYRES_TYPE_CreateLayout	'l'
#define MYRES_TYPE_CreateVS		'v'
#define MYRES_TYPE_CreatePS		'p'
#define MYRES_TYPE_CreateTex	't'
#define MYRES_TYPE_CreateSample	's'
#define MYRES_TYPE_CreateBlend		'a'
#define MYRES_TYPE_CreateDapth	'd'
#define MYRES_TYPE_D3D9_VB		'k'
#define MYRES_TYPE_D3D9_IB			'm'
#define MYRES_TYPE_CreateAnim		'c'

struct Matrix3x4f
{
	float a[12];
	Matrix4f ToMat()
	{
		Matrix4f mat;
		for (int i = 0; i < 12; ++i)
			mat[i] = a[i];
		mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;
		return mat;
	}
};

struct Matrix3x4n
{
	Vector4n a;
	Vector4n b;
	Vector4n c;
	Matrix4f ToMat()
	{
		Matrix4f mat;
		mat[0] = (float)a.x / 100.0f; mat[1] = (float)a.y / 100.0f; mat[2] = (float)a.z / 100.0f; mat[3] = (float)a.w / 100.0f;
		mat[4] = (float)b.x / 100.0f; mat[5] = (float)b.y / 100.0f; mat[6] = (float)b.z / 100.0f; mat[7] = (float)b.w / 100.0f;
		mat[8] = (float)c.x / 100.0f; mat[9] = (float)c.y / 100.0f; mat[10] = (float)c.z / 100.0f; mat[11] = (float)c.w / 100.0f;
		mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;
		return mat;
	}
};

struct CBMain
{
	Vector4f Constant0;                  // Offset:    0 Size:    16 [unused]
	Vector4f Constant1;                  // Offset:   16 Size:    16 [unused]
	Matrix4f matWorld;       // Offset:   32 Size:    64
	Matrix4f matView;        // Offset:   96 Size:    64 [unused]
	Matrix4f matWorldView;   // Offset:  160 Size:    64
	Matrix4f matWorldViewProj;// Offset:  224 Size:    64
	Vector4f fogParams;                  // Offset:  288 Size:    16
	Matrix4f matTex[12];        // Offset:  304 Size:    64 [unused]
	//Matrix4f matTex1;        // Offset:  368 Size:    64
	//Matrix4f matTex2;        // Offset:  432 Size:    64
	//Matrix4f matTex3;        // Offset:  496 Size:    64
	//Matrix4f matTex4;        // Offset:  560 Size:    64
	//Matrix4f matTex5;        // Offset:  624 Size:    64
	//Matrix4f matTex6;        // Offset:  688 Size:    64
	//Matrix4f matTex7;        // Offset:  752 Size:    64
	//Matrix4f matTex8;        // Offset:  816 Size:    64 [unused]
	//Matrix4f matTex9;        // Offset:  880 Size:    64
	//Matrix4f matTex10;       // Offset:  944 Size:    64 [unused]
	//Matrix4f matTex11;       // Offset: 1008 Size:    64 [unused]
	Vector4f viewport;                   // Offset: 1072 Size:    16 [unused]
	Vector4f windowResolution;           // Offset: 1088 Size:    16 [unused]
	Vector4f waveParams;                 // Offset: 1104 Size:    16 [unused]
	Vector4f edgealphaParams;            // Offset: 1120 Size:    16 [unused]
	Vector4f wpPlayer;                   // Offset: 1136 Size:    16
	Vector4f wvScreenZAxis;              // Offset: 1152 Size:    16 [unused]
	Vector4f wpViewPos;                  // Offset: 1168 Size:    16 [unused]
	Vector4f Time;                       // Offset: 1184 Size:    16 [unused]
	Vector4f MeshLighting;               // Offset: 1200 Size:    16 [unused]
	Vector4f Factor;                     // Offset: 1216 Size:    16 [unused]
	Vector4f ShadowColor;                // Offset: 1232 Size:    16 [unused]
	Vector4f SpecularPower;              // Offset: 1248 Size:    16 [unused]
	Vector4f colLight;                   // Offset: 1264 Size:    16 [unused]
	Vector4f Weights1;                   // Offset: 1280 Size:    16 [unused]
	Vector4f Weights2;                   // Offset: 1296 Size:    16 [unused]
	Vector4f Weights3;                   // Offset: 1312 Size:    16 [unused]
	Vector4f Weights4;                   // Offset: 1328 Size:    16 [unused]
	Vector4f WaveConstants;              // Offset: 1344 Size:    16 [unused]
	Vector4f fogColor;                   // Offset: 1360 Size:    16 [unused]
	Vector4f diffuseOverlay;             // Offset: 1376 Size:    16 [unused]
	Vector4f appearanceFX;               // Offset: 1392 Size:    16 [unused]
	Vector4f depthOfFieldParams;         // Offset: 1408 Size:    16 [unused]
	Vector4f cameraParams;               // Offset: 1424 Size:    16 [unused]
	Vector4f depthReconstruction;        // Offset: 1440 Size:    16 [unused]
	Vector4f edgeFilter;                 // Offset: 1456 Size:    16 [unused]
	Vector4f blurConstants;              // Offset: 1472 Size:    16 [unused]
	Vector4f waterTextureConstants;      // Offset: 1488 Size:    16 [unused]
	Vector4f hitFlashColorAdd;           // Offset: 1504 Size:    16 [unused]
	Vector4f hitFlashColorMul;           // Offset: 1520 Size:    16 [unused]
	Vector4f tintRampUV;                 // Offset: 1536 Size:    16 [unused]
	Vector4f heroTintColorB;             // Offset: 1552 Size:    16 [unused]
	Vector4f weatherIntensity;           // Offset: 1568 Size:    16 [unused]
	Vector4f numBatches;                 // Offset: 1584 Size:    16 [unused]
	Vector4f randFloat;                  // Offset: 1600 Size:    16 [unused]
	Vector4f bUseDyeType;                // Offset: 1616 Size:    16 [unused]
	Vector4f deformationScale;           // Offset: 1632 Size:    16 [unused]
	Vector4f colorCorrectionBlend;       // Offset: 1648 Size:    16 [unused]
	Vector4f targetDimensions;           // Offset: 1664 Size:    16 [unused]
	Vector4f normalMapScale;             // Offset: 1680 Size:    16 [unused]
	Vector4f bannerTintRampUV;           // Offset: 1696 Size:    16 [unused]
	Vector4f bannerPatternTint;          // Offset: 1712 Size:    16 [unused]
	Vector4f bannerSigilTint;            // Offset: 1728 Size:    16 [unused]
	Vector4f bannerPatternDuotoneTint;   // Offset: 1744 Size:    16 [unused]
	Vector4f dbgReg0;                    // Offset: 1760 Size:    16 [unused]
	Vector4f lightMapVScale;             // Offset: 1776 Size:    16 [unused]
	Vector4f vecFixedPointPosScale;      // Offset: 1792 Size:    16 [unused]
	Vector4f vecFixedPointPosOffset;     // Offset: 1808 Size:    16 [unused]
	Matrix4f matWorldIrrad;  // Offset: 1824 Size:    64 [unused]
	Vector4f alphaTestRef;               // Offset: 1888 Size:    16 [unused]
	Vector4f qCameraBillboard;           // Offset: 1904 Size:    16 [unused]
	Vector4f billboardParams;            // Offset: 1920 Size:    16 [unused]
	Matrix4f matProjection;  // Offset: 1936 Size:    64 [unused]
	Vector4f vecClipPlane[6];            // Offset: 2000 Size:    96
	Vector4f vecReflectionPlane[2];      // Offset: 2096 Size:    32 [unused]
	Matrix4f matEffect1;     // Offset: 2128 Size:    64 [unused]
	Matrix4f matEffect2;     // Offset: 2192 Size:    64 [unused]
	Matrix4f matSSAOViewProj;// Offset: 2256 Size:    64 [unused]
	Matrix4f matColorTransform;// Offset: 2320 Size:    64 [unused]

};



struct MyResBase
{
	unsigned int type;
	void * addr;
	unsigned int crc;
	unsigned int totalSize;
	int reserve1;
	int reserve2;
	int reserve3;
};

typedef enum _MyD3DDECLUSAGE {
	D3DDECLUSAGE_POSITION = 0,
	D3DDECLUSAGE_BLENDWEIGHT = 1,
	D3DDECLUSAGE_BLENDINDICES = 2,
	D3DDECLUSAGE_NORMAL = 3,
	D3DDECLUSAGE_PSIZE = 4,
	D3DDECLUSAGE_TEXCOORD = 5,
	D3DDECLUSAGE_TANGENT = 6,
	D3DDECLUSAGE_BINORMAL = 7,
	D3DDECLUSAGE_TESSFACTOR = 8,
	D3DDECLUSAGE_POSITIONT = 9,
	D3DDECLUSAGE_COLOR = 10,
	D3DDECLUSAGE_FOG = 11,
	D3DDECLUSAGE_DEPTH = 12,
	D3DDECLUSAGE_SAMPLE = 13
} MyD3DDECLUSAGE;
typedef enum _MyD3DDECLTYPE
{
	D3DDECLTYPE_FLOAT1 = 0,  // 1D float expanded to (value, 0., 0., 1.)
	D3DDECLTYPE_FLOAT2 = 1,  // 2D float expanded to (value, value, 0., 1.)
	D3DDECLTYPE_FLOAT3 = 2,  // 3D float expanded to (value, value, value, 1.)
	D3DDECLTYPE_FLOAT4 = 3,  // 4D float
	D3DDECLTYPE_D3DCOLOR = 4,  // 4D packed unsigned bytes mapped to 0. to 1. range
	// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	D3DDECLTYPE_UBYTE4 = 5,  // 4D unsigned byte
	D3DDECLTYPE_SHORT2 = 6,  // 2D signed short expanded to (value, value, 0., 1.)
	D3DDECLTYPE_SHORT4 = 7,  // 4D signed short

// The following types are valid only with vertex shaders >= 2.0


D3DDECLTYPE_UBYTE4N = 8,  // Each of 4 bytes is normalized by dividing to 255.0
D3DDECLTYPE_SHORT2N = 9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
D3DDECLTYPE_SHORT4N = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
D3DDECLTYPE_USHORT2N = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
D3DDECLTYPE_USHORT4N = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
D3DDECLTYPE_UDEC3 = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
D3DDECLTYPE_DEC3N = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
D3DDECLTYPE_UNUSED = 17,  // When the type field in a decl is unused.
} MyD3DDECLTYPE;
typedef struct _MyD3DVERTEXELEMENT9
{
	WORD    Stream;     // Stream index
	WORD    Offset;     // Offset in the stream in bytes
	BYTE    Type;       // Data type
	BYTE    Method;     // Processing method
	BYTE    Usage;      // Semantics
	BYTE    UsageIndex; // Semantic index
} MyD3DVERTEXELEMENT9;
typedef struct _MyD3DSURFACE_DESC
{
	UINT				Format;
	UINT				Type;
	DWORD         Usage;
	UINT             Pool;

	UINT				MultiSampleType;
	DWORD         MultiSampleQuality;
	UINT              Width;
	UINT              Height;
} MyD3DSURFACE_DESC;
typedef struct _MyD3DVERTEXBUFFER_DESC
{
	UINT           Format;
	UINT     Type;
	DWORD               Usage;
	UINT             Pool;
	UINT                Size;

	DWORD               FVF;

} MyD3DVERTEXBUFFER_DESC;
struct MyRes_D3D9_VB
{
	MyResBase head;
	MyD3DVERTEXBUFFER_DESC desc;
	char data[];
};
typedef struct _MyD3DINDEXBUFFER_DESC
{
	UINT           Format;
	UINT     Type;
	DWORD               Usage;
	UINT             Pool;
	UINT                Size;
} MyD3DINDEXBUFFER_DESC;
struct MyRes_D3D9_IB
{
	MyResBase head;
	MyD3DINDEXBUFFER_DESC desc;
	char data[];
	void* CreateResource();
};

struct MyRes_CreateBuffer
{
	MyResBase head; // reserve1 = vertexstride;
	D3D11_BUFFER_DESC desc;
	char data[];
	void* CreateResource();
};

struct MyRes_CreateLayout
{
	MyResBase head; // reserve1 = numElements;
	char data[];
	void SetNameOffset();
	unsigned int GetStride(int slotIndex);
	unsigned int GetTextureOffset(int _index);
	unsigned int GetMatIdxOffset();
	unsigned int GetMatWeightOffset();
	void* CreateResource(void* _bolb, int _size);
};

struct MyRes_CreateShader
{
	MyResBase head;
	char data[];
	void* CreateResource();
};

struct MyRes_CreateTexture
{
	MyResBase head;
	//pData->head.reserve1 = pInitialData->SysMemPitch;
	//pData->head.reserve2 = pInitialData->SysMemSlicePitch;

	D3D11_TEXTURE2D_DESC desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	D3D11_SUBRESOURCE_DATA subRes[64];
	char data[];
	void SetSubResMem();
	unsigned int GetCount() { return 64; }
	void* CreateResource(int width, int height, char *imgTGA);
};

struct MyRes_CreateSS
{
	MyResBase head;
	D3D11_SAMPLER_DESC desc;
	void* CreateResource();
};

struct MyRes_CreateBS
{
	MyResBase head;
	D3D11_BLEND_DESC desc;
	void* CreateResource();
};

struct MyRes_CreateDS
{
	MyResBase head;
	D3D11_DEPTH_STENCIL_DESC desc;
	void* CreateResource();
};
struct MyRes_CreateAnimations
{
	MyResBase head;
	int offset[16];
	int counts[16];
	char data[];
};






struct VertexType_Texture
{
	Vector3f p;
	Vector2f t;
};
struct VertexType_Weight
{
	Vector3f p;
	Vector2f t;
	Vector3f n;
	Vector4n index;
	Vector4f weight;
};
struct VertexType_Sprite
{
	Vector3f p;
	Vector2f t;
};
struct VertexType_Color
{
	Vector3f p;
	Vector4f c;
};
struct VertexType_Diablo
{
	Vector3f p;
	char n[4];
	char c1[4];
	char c2[4];
	char t1[4];
	char t2[4];
};

struct MyBuffer {
	char type;
	D3D11_BUFFER_DESC desc;
	char data[];
};
struct MyShader {
	char type;
	char data[];
};
struct MyLayout {
	char type;
	char numEle;
	D3D11_INPUT_ELEMENT_DESC desc[];
};


struct TexelVector //should be 16byte aligned
{
	Vector4f vectors[12];
};
struct MatrixBufferType //should be 16byte aligned
{
	Matrix4f world;
	Matrix4f view;
	Matrix4f projection;
};
struct MatrixBoneBufferType //should be 16byte aligned
{
	Matrix4f world;
	Matrix4f view;
	Matrix4f projection;
	Matrix4f bones[45];
};
struct MaterialBufferType //should be 16byte aligned
{
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	Vector4f shininess;
};
struct LightBufferType //should be 16byte aligned
{
	Vector4f position;
	Vector4f direction;
	Vector4f color;
	Vector4f reserve;
};

#pragma pack(pop)

struct VBInfo
{
	void *addr;
	UINT numBuf;
	UINT strides[32]; //numBuffers
	UINT offset[32]; //numBuffers
	bool isDirty;
};
struct TEXInfo
{
	void *addr;
	UINT NumViews;
	bool isDirty;
};
struct SSInfo
{
	void *addr;
	UINT NumSamplers;
	bool isDirty;
};
struct MapInfo
{
	void *addr;
	UINT subRes;
	UINT type;
	UINT flags;
	D3D11_MAPPED_SUBRESOURCE mappedSubRes;
	bool isDirty;
};
struct RenderContext
{
	VBInfo vb[32];

	void *ib_addr;
	UINT ib_format;
	UINT ib_offset;
	bool ib_isDirty;

	UINT prim_topology;
	bool prim_isDirty;

	MapInfo CBMain;
	MapInfo CBBones;
	MapInfo CBLights;

	TEXInfo tex[32];

	void *layout_addr;
	bool layout_isDirty;

	void *vs_addr;
	UINT vs_NumClassInstances;
	void *vs_pClassInstances;
	bool vs_isDirty;

	void *ps_addr;
	UINT ps_NumClassInstances;
	void *ps_pClassInstances;
	bool ps_isDirty;

	void *bs_addr;
	float bs_factor[4];
	UINT bs_mask;
	bool bs_isDirty;

	void *ds_addr;
	UINT ds_ref;
	bool ds_isDirty;

	SSInfo ss[32];

	UINT draw_IndexCount;
	UINT draw_StartIndex;
	UINT draw_BaseVertex;
	bool draw_isDirty;
};
struct D3D9_RenderContext
{
	UINT context_Index;
	char context_filename[64];
	UINT decl_count;
	MyD3DVERTEXELEMENT9 decl_eles[32];

	void* vb_addr;
	UINT vb_streamNum;
	UINT vb_OffsetBytes;
	UINT vb_stride;
	MyD3DVERTEXBUFFER_DESC vb_desc;
	bool vb_isDirty;

	void* ib_addr;
	MyD3DINDEXBUFFER_DESC ib_desc;
	bool ib_isDirty;

	DWORD fvf_fvf;
	bool fvf_isDirty;

	UINT draw_primitiveType;
	INT draw_BaseVertexIndex;
	UINT draw_MinVertexIndex;
	UINT draw_NumVertices;
	UINT draw_startIndex;
	UINT draw_primCount;
	bool draw_isDirty;

	void* tex_addr[32];
	MyD3DSURFACE_DESC tex_desc[32];
	bool tex_isDirty[32];

	int GetStride();
	int GetPositionOffset(MyD3DDECLTYPE& type);
	int GetTexelOffset(int idx, MyD3DDECLTYPE& type);
	int GetNormalOffset(MyD3DDECLTYPE& type);
};