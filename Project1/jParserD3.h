#pragma once

#include "junks.h"
#include "jDirectXheader.h"
#include "jParserSub.h"

#define PATH_EXPORT "D:\\export\\"

struct MyResBase;

struct GeometryInfo
{
	int vertexTotalCount;
	int vertexStride;
	int vertexVertexByteOffset;
	int indiciesIndexUnit;// 2
	int indiciesCntPerPoly;// 3
	int indiciesTotalSize;
	int primitiveType;
	int drawIndexCount;
	int drawVertCount;
	int drawIndexOffset;
	int drawVertOffset;
};
struct ExpVertex
{
	Vector3f p;
	Vector3f n;
	Vector2f t;
};
struct ExpMetaInfo
{
	vector<string> images;
	vector<Vector3f> vectors;
	Vector3f worldPosition;
};
struct ExpMesh
{
	string name;
	ExpMetaInfo metaInfo;
	vector<ExpVertex> vert;
	vector<Vector3n> indicies;
	ExpMesh *pNext;
	bool ExportMetaInfo(string path);
	bool ExportToObject(string _filename, bool _isRoot, int _baseIdx);
	bool Merge(ExpMesh *_mesh);
	bool Add(int _idx, ExpVertex& _vert);
	ExpMesh()
	{
		name = "";
		pNext = nullptr;
	}
	void Reset()
	{
		name = "";
		pNext = nullptr;
		vert.clear();
		indicies.clear();
	}
};

class jParserD3
{
public:
	static map<UINT64, ExpMesh*> mExpLinks;
	static map<void*, pair<MyResBase*, void*>> mMapRes;
	static void LoadResources(int idx);
	static void Release();
	
	
	jParserD3();
	virtual ~jParserD3();

	GeometryInfo mGeoInfo;
	int mFileIndex;
	function< Vector2f(int _idx, unsigned char *_p) > mFuncConvertTex;
	RenderContext mContext;
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
	virtual ID3D11ShaderResourceView* GetResShaderResourceView(int _idx);
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
	virtual char* GetIndiciesData();
	virtual void* GetTexResAddr(int _idx);

	Vector2f CalcTexCoord(unsigned char* _p)
	{
		Vector2f ret;
		ret.x = ((float)_p[0] * 0.003906f + (float)_p[1]) * 0.5f - 64.0f;
		ret.y = ((float)_p[2] * 0.003906f + (float)_p[3]) * 0.5f - 64.0f;
		return ret;
	}
	bool IsTerrain();
	unsigned long long CalcKey(float _x, float _y);
	ExpMesh mExportInfo;
	bool ExportToObjectFormat();
	bool ExportTerrains();
	bool AddVertInfo(int index, int offset);
};

