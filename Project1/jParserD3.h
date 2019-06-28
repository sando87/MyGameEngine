#pragma once
#include <utility>
#include <map>
#include <functional>

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
	int mVertexStride;
	int mVertexOffset;
	int mVertexCount;
	function< Vector2f(int _idx, unsigned char *_p) > mFuncConvertTex;
	vector<int> mTextures;
	CBMain mCBMain;
	MyRes_CreateBuffer* mpVerticies;
	int mLayoutFileID;

	bool Init(int _fileIdx);
	void InitFuncConvTex();
	void ReadyForData();
	void InitTextureList();
	void InitCBMain();
	bool IsValid();
	
	void* CreateD3DRescource(void* addr);
	ID3D11Buffer* GetResIndexBuffer();
	ID3D11BlendState* GetResBlendState();
	ID3D11DepthStencilState* GetResDepthStencilState();
	ID3D11ShaderResourceView* GetResShaderResourceView();
	bool SetContantBuffer(MapInfo& cb, int slotIdx);
	bool Render();
	
	
	Vector3f GetPos(int _idx);
	Vector3f GetNor(int _idx);
	int GetTex(int _idx, Vector2f* _t);
	Vector4n GetMatIdx(int _idx);
	Vector4f GetMatWeight(int _idx);

	Vector2f CalcTexCoord(unsigned char* _p)
	{
		Vector2f ret;
		ret.x = ((float)_p[0] * 0.003906f + (float)_p[1]) * 0.5f - 64.0f;
		ret.y = ((float)_p[2] * 0.003906f + (float)_p[3]) * 0.5f - 64.0f;
		return ret;
	}
};

