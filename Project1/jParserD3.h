#pragma once
#include <utility>
#include <map>

#include "jGlobalStruct.h"
using namespace std;
struct MyResBase;

class jParserD3
{
public:
	static map<void*, pair<MyResBase*, void*>> mMapRes;
	static void LoadResources();
	static void Release();
	
	
	jParserD3();
	~jParserD3();

	int mFileIndex;
	RenderContext mContext;
	int mVertBufIdx;
	int mVertexStride;
	int mVertexOffset;
	int mVertexCount;

	int mTexCoordIndex;
	int mTextureIndex;
	
	void Init(int _fileIdx);
	void CreateD3DRescource(void* addr);
	CBMain GetCBMain();

	ID3D11Buffer* GetResIndexBuffer();
	ID3D11ShaderResourceView* GetResShaderResourceView();
	
	void SetTexCoordIndex();
	void SetTextureIndex();
	void SetVertexBuffer(int _idx) { mVertBufIdx = _idx; }
	Vector3f GetPos(int _idx);
	Vector3f GetNor(int _idx);
	Vector2f GetTex(int _idx);
	Vector2f ConvertTex(unsigned char *_p);
	Vector4n GetMatIdx(int _idx);
	Vector4f GetMatWeight(int _idx);
};

