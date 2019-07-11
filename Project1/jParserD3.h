#pragma once
#include <utility>
#include <map>
#include <functional>

#include "jGlobalStruct.h"
using namespace std;
struct MyResBase;

struct GeometryInfo
{
	unsigned int vertexTotalCount;
	unsigned int vertexStride;
	unsigned int vertexVertexByteOffset;
	unsigned int indiciesIndexUnit;// 2
	unsigned int indiciesCntPerPoly;// 3
	unsigned int indiciesTotalSize;
	unsigned int primitiveType;
	unsigned int drawIndexCount;
	unsigned int drawVertCount;
	unsigned int drawIndexOffset;
	unsigned int drawVertOffset;
};
struct ExpVertInfo
{
	string p;
	string n;
	string t;
};

class jParserD3
{
public:
	static map<void*, pair<MyResBase*, void*>> mMapRes;
	static void LoadResources(int idx);
	static void Release();
	
	
	jParserD3();
	virtual ~jParserD3();

	GeometryInfo mGeoInfo;
	int mFileIndex;
	RenderContext mContext;
	function< Vector2f(int _idx, unsigned char *_p) > mFuncConvertTex;
	vector<int> mTextures;
	CBMain mCBMain;
	MyRes_CreateBuffer* mpVerticies;
	int mLayoutFileID;

	virtual bool Init(int _fileIdx);
	virtual bool InitGeoInfo();
	void InitFuncConvTex();
	void ReadyForData();
	void InitTextureList();
	void InitCBMain();
	bool IsValid();
	
	void* CreateD3DRescource(void* addr);
	virtual ID3D11Buffer* GetResIndexBuffer();
	virtual ID3D11ShaderResourceView* GetResShaderResourceView();
	ID3D11BlendState* GetResBlendState();
	ID3D11DepthStencilState* GetResDepthStencilState();
	bool SetContantBuffer(MapInfo& cb, int slotIdx);
	bool Render();
	GeometryInfo GetGeometryInfo() { return mGeoInfo; }
	
	
	virtual Vector3f GetPos(int _idx, int byteOffset = 0);
	virtual Vector3f GetNor(int _idx, int byteOffset = 0);
	virtual int GetTex(int _idx, Vector2f* _t, int byteOffset = 0);
	virtual Vector4n GetMatIdx(int _idx);
	virtual Vector4f GetMatWeight(int _idx);

	Vector2f CalcTexCoord(unsigned char* _p)
	{
		Vector2f ret;
		ret.x = ((float)_p[0] * 0.003906f + (float)_p[1]) * 0.5f - 64.0f;
		ret.y = ((float)_p[2] * 0.003906f + (float)_p[3]) * 0.5f - 64.0f;
		return ret;
	}
	map<int, ExpVertInfo> mMapExpVertInfo;
	int mMaxKey;
	virtual bool ExportToObjectFormat();
	bool AddVertInfo(int index, int offset);
};

