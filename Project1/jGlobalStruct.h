#pragma once

#include "header.h"
#include "Vectors.h"
#include "Matrices.h"

#pragma pack(push, 1)

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

struct CBMain
{
	Vector4f Constant0;                  // Offset:    0 Size:    16 [unused]
	Vector4f Constant1;                  // Offset:   16 Size:    16 [unused]
	Matrix4f matWorld;       // Offset:   32 Size:    64
	Matrix4f matView;        // Offset:   96 Size:    64 [unused]
	Matrix4f matWorldView;   // Offset:  160 Size:    64
	Matrix4f matWorldViewProj;// Offset:  224 Size:    64
	Vector4f fogParams;                  // Offset:  288 Size:    16
	Matrix4f matTex0;        // Offset:  304 Size:    64 [unused]
	Matrix4f matTex1;        // Offset:  368 Size:    64
	Matrix4f matTex2;        // Offset:  432 Size:    64
	Matrix4f matTex3;        // Offset:  496 Size:    64
	Matrix4f matTex4;        // Offset:  560 Size:    64
	Matrix4f matTex5;        // Offset:  624 Size:    64
	Matrix4f matTex6;        // Offset:  688 Size:    64
	Matrix4f matTex7;        // Offset:  752 Size:    64
	Matrix4f matTex8;        // Offset:  816 Size:    64 [unused]
	Matrix4f matTex9;        // Offset:  880 Size:    64
	Matrix4f matTex10;       // Offset:  944 Size:    64 [unused]
	Matrix4f matTex11;       // Offset: 1008 Size:    64 [unused]
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

#pragma pack(pop)
