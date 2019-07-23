#pragma once
#include "jParserD3.h"

class jParserS2 : public jParserD3
{
public:
	jParserS2();
	~jParserS2();

	D3D9_RenderContext mContextD3D9;

	bool Init(int _fileIdx);

	ID3D11Buffer* GetResIndexBuffer();
	ID3D11ShaderResourceView* GetResShaderResourceView(int _idx);
	bool InitGeoInfo();

	Vector3f GetPos(int _idx, int byteOffset = 0);
	Vector3f GetNor(int _idx, int byteOffset = 0);
	int GetTex(int _idx, Vector2f* _t, int byteOffset = 0);
	Vector4n GetMatIdx(int _idx);
	Vector4f GetMatWeight(int _idx);
	char * GetIndiciesData();
	void* GetTexResAddr(int _idx);
};

