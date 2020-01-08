#include "jParserD3.h"
#include "jRenderer.h"
#include "jShaderHeader.h"

#define REV_V(v) ((v))
#define RES_ID(crc, size) (((size)<<8) | (crc))
#define COMP_FLOAT(a, b) (abs((a)-(b)) < 0.0001f ? true : false)

map<void*, pair<MyResBase*, void*>> jParserD3::mMapRes;
map<UINT64, ExpMesh*> jParserD3::mExpLinks;

struct LayoutA
{
	float p[3];
	unsigned char n[4];
	unsigned char c0[4];
	unsigned char c1[4];
	unsigned char t0[4];
	unsigned char t1[4];
	unsigned char i[4];
	float w[3];
};
struct LayoutB
{
	float p[3];
	unsigned char n[4];
	unsigned char c0[4];
	unsigned char c1[4];
	unsigned char t0[4];
	unsigned char t1[4];
};
struct LayoutC
{
	float p[3];
	unsigned char c0[4];
	unsigned char c1[4];
	unsigned char t0[4];
	unsigned char t1[4];
	unsigned char t2[4];
	unsigned char t3[4];
	unsigned char t4[4];
	float t5[4];
};
struct LayoutD
{
	float p[3];
	unsigned char c0[4];
	unsigned char c1[4];
	float t0[2];
	float t1[2];
	float t2[2];
};
struct LayoutE
{
	float p[3];
	unsigned char n[4];
	unsigned char c0[4];
	unsigned char c1[4];
	unsigned char t0[4];
	unsigned char t1[4];
	//float t2[3];
};
struct LayoutF
{
	float p[3];
	unsigned char n[4];
	unsigned char c0[4];
	unsigned char c1[4];
	unsigned char t0[4];
};
//D3D11_INPUT_ELEMENT_DESC


jParserD3::jParserD3()
{
	mpVerticies = nullptr;
	mLayoutFileID = 0;
}


jParserD3::~jParserD3()
{
}


void jParserD3::LoadResources(int idx)
{
	string path = PATH_PARSER_DATA;
	string filter = "*.dump";
	jUtils::ForEachFiles2(nullptr, (path + filter).c_str(), [&](void* _obj, string _str) {

		vector<string> rets;
		jUtils::Split(_str, "_.", rets);
		unsigned long long nAddr = 0;
		stringstream ss;
		ss << std::hex << rets[idx];
		ss >> nAddr;
		void* id = (void*)nAddr;
		if (mMapRes.find(id) != mMapRes.end() || rets[idx + 1] == "c")
			return true;

		int filesize = 0;
		MyResBase* pBuf = nullptr;
		bool ret = jUtils::LoadFile(path + _str, &filesize, (char**)&pBuf);
		if (!ret || pBuf == nullptr)
		{
			_warn();
			return true;
		}

		mMapRes[id] = make_pair(pBuf, nullptr);
		return true;
	});


	//path = PATH_PARSER_DATA;
	//filter = "*_RenderingContext.bin";
	//jUtils::ForEachFiles2(nullptr, (path + filter).c_str(), [&](void* _obj, string _str) {
	//
	//	vector<string> rets;
	//	jUtils::Split(_str, "_.", rets);
	//	int idx = atoi(rets[0].c_str());
	//
	//	int filesize = 0;
	//	RenderContext* pBuf = nullptr;
	//	jUtils::LoadFile(path + _str, &filesize, (char**)&pBuf);
	//
	//	free(pBuf);
	//	return KEEPGOING;
	//});

}
void jParserD3::Release()
{
	//for (auto iter = mMapRes.begin(); iter != mMapRes.end(); ++iter)
	//{
	//	auto item = iter->second;
	//	void* pData = item.first;
	//	void* d3dRes = item.second;
	//	free(pData);
	//	//d3dRes->Release();
	//}
	//mMapRes.clear();
}



bool jParserD3::Init(int _fileIdx)
{
	LoadResources(1);

	mFileIndex = _fileIdx;
	string name = PATH_PARSER_DATA + to_string(_fileIdx) + "_RenderingContext.bin";
	int size = 0;
	char* pBuf = nullptr;
	jUtils::LoadFile(name, &size, (char**)&pBuf);
	memcpy(&mContext, pBuf, sizeof(mContext));

	InitCBMain();
	ReadyForData();
	InitFuncConvTex();
	InitTextureList();
	InitGeoInfo();
	InitAnims();

	if (!IsValid())
	{
		_warn();
		return false;
	}

	return true;
}
bool jParserD3::InitGeoInfo()
{
	if (mpVerticies == nullptr)
		return false;
	
	int vertexBufSize = mpVerticies->head.totalSize - sizeof(MyRes_CreateBuffer);
	mGeoInfo.vertexStride = mContext.vb[0].strides[0];
	mGeoInfo.vertexVertexByteOffset = mContext.vb[0].offset[0];
	mGeoInfo.vertexTotalCount = vertexBufSize / mGeoInfo.vertexStride;

	void *pAddr = mContext.ib_addr;
	MyRes_CreateBuffer *pIndicies = (MyRes_CreateBuffer*)mMapRes[pAddr].first;
	mGeoInfo.indiciesIndexUnit = 2;
	mGeoInfo.indiciesCntPerPoly = 3;
	mGeoInfo.indiciesTotalSize = pIndicies->desc.ByteWidth;
	mGeoInfo.primitiveType = mContext.prim_topology; // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	mGeoInfo.drawIndexCount = mContext.draw_IndexCount;
	mGeoInfo.drawVertCount = 0;
	mGeoInfo.drawIndexOffset = mContext.draw_StartIndex;
	mGeoInfo.drawVertOffset = mContext.draw_BaseVertex;
	return true;
}
void jParserD3::InitFuncConvTex()
{
	if (IsTerrain())
	{
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Vector2f ret = CalcTexCoord(_p);
			return Vector2f(ret.x, REV_V(ret.y));
		};
		return;
	}

	MyRes_CreateShader* pData = (MyRes_CreateShader*)mMapRes[mContext.vs_addr].first;
	int resID = RES_ID(pData->head.crc, pData->head.totalSize);
	switch (resID)
	{
	case RES_ID(0x53, 0x1318):	//0  
	case RES_ID(0x1f, 0x1948):	//122
	case RES_ID(0xd5, 0x1bd0):	//125
	case RES_ID(0x1a, 0x17a4):	//126
	case RES_ID(0xd1, 0x1848):	//127
	case RES_ID(0x77, 0x1804):	//130
	case RES_ID(0xa0, 0x23a4):	//16 
	case RES_ID(0xd7, 0x1704):	//17 
	case RES_ID(0xCB, 0x11BC): //1  
	case RES_ID(0x80, 0x2264):	//48 
	case RES_ID(0x6a, 0x1c64):	//72 
	case RES_ID(0x16, 0x1f5c):	//75 
	case RES_ID(0xa1, 0x1268): //138-2
	case RES_ID(0xa1, 0x1644): //139-2
	case RES_ID(0x7c, 0x1ad0): //193-2
	case RES_ID(0xc1, 0x14d4): //182-2
	case RES_ID(0x9d, 0x14dc): //190-2
	case RES_ID(0xb4, 0x1ddc): //198-2 
	case RES_ID(0xd3, 0x1a5c): //200-2 
	case RES_ID(0x0, 0x12dc): //261-2
	case RES_ID(0x3c, 0x1744): //269-2
	case RES_ID(0xc1, 0x1518):
	case RES_ID(0x85, 0x150c):
	case RES_ID(0xe3, 0x11dc):
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[_idx];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x50, 0x1ee4):
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[_idx];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			if (_idx >= 2)
				ret = tmp;
			else
			{
				ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
				ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			}
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0xfc, 0x25b4):	//42 
	case RES_ID(0x9d, 0x22a8):	//43 
	case RES_ID(0x86, 0x22c4):	//44 
	case RES_ID(0xc7, 0x1fa0): //69-1
	case RES_ID(0x17, 0x1fbc): //70-1
	case RES_ID(0x18, 0x22ac):	//41, 74-1
		mFuncConvertTex = [&](int _idx, unsigned char* _p) { 
			Matrix4 matTex = mCBMain.matTex[0];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x67, 0x1e34):	//138
	case RES_ID(0x79, 0x13c8):	//150
	case RES_ID(0x63, 0x18a8):	//21 	
	case RES_ID(0x9a, 0x200c):	//34 
	case RES_ID(0x41, 0x237c):	//38 
	case RES_ID(0xff, 0x18a8):	//82 
	case RES_ID(0xd2, 0x1a48): //13-1
	case RES_ID(0xd5, 0x2014):
	case RES_ID(0xad, 0x23e8):
	case RES_ID(0x7, 0x2100):
	case RES_ID(0x49, 0x23f0):
	case RES_ID(0xc5, 0x18b0):
	case RES_ID(0x4c, 0x2038):
	case RES_ID(0x8c, 0x1a04):
	case RES_ID(0x84, 0x1A30):
	case RES_ID(0xed, 0x1b8c):
	case RES_ID(0x51, 0x1a30):
	case RES_ID(0xc2, 0x1c4c): 
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[_idx + 1];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x89, 0xfe4):	//108
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Vector2f *pfactor = (Vector2f *)&mCBMain.Constant0;
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x + pfactor[_idx].x;
			ret.y = tmp.y + pfactor[_idx].y;
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0xd0, 0xf10):	//109
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Vector2f ret = CalcTexCoord(_p);
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x7a, 0x1468):	//151
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Vector4f factor = mCBMain.viewport;
			Matrix4 matTex = mCBMain.matTex[0];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f tmp2;
			tmp2.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			tmp2.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			Vector2f ret;
			ret.x = tmp2.x * factor.x + factor.y;
			ret.x = tmp2.y * factor.z + factor.w;
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x80, 0x1158):	//18 
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			float *p = (float *)_p;
			Vector2f ret;
			ret.x = p[0];
			ret.y = p[1];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x28, 0x121c):	//152
	case RES_ID(0x8e, 0x1168):
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			float* p = (float*)_p;
			Matrix4f matTex = mCBMain.matTex[0];
			Vector2f ret;
			ret.x = p[0] * matTex[0] + p[1] * matTex[1] + (1.0f) * matTex[3];
			ret.y = p[0] * matTex[4] + p[1] * matTex[5] + (1.0f) * matTex[7];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0xcb, 0x22a8): //169-2
	case RES_ID(0x47, 0x1a18): //171-2
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Vector2f ret = CalcTexCoord(_p);
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x2c, 0x1240):	//157
	case RES_ID(0x3a, 0x1564):	//186
	case RES_ID(0xff, 0xfcc):	//220
	case RES_ID(0xd6, 0x1e50): //139
		_printlog("[vs] default : %d[idx], 0x%x[crc], 0x%x[size]\n", mFileIndex, pData->head.crc, pData->head.totalSize);
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[_idx];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x6f, 0x2170):
	case RES_ID(0xd1, 0x2150):
		_printlog("[vs] skip too hard.. : %d[idx], 0x%x[crc], 0x%x[size]\n", mFileIndex, pData->head.crc, pData->head.totalSize);
		break;
	case RES_ID(0x92, 0x1378):
		_printlog("[vs] skip : pos * view mat needed!! \n"); // texel 변환 좌표가 view좌표계 기준 변환 필요
		break;
	case RES_ID(0x90, 0x142c):
		_printlog("[vs] skip : color * view mat needed!! \n"); // texel 변환 좌표가 view좌표계 기준 변환 필요
		break;
	default:
		_printlog("[vs] undefine : %d[idx], 0x%x[crc], 0x%x[size]\n", mFileIndex, pData->head.crc, pData->head.totalSize);
		break;
	}

	if (mFuncConvertTex == nullptr)
	{
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			return Vector2f(0, 0);
		};
	}
}
void jParserD3::ReadyForData()
{
	int vertBufIdx = 0;
	void *pAddr = mContext.vb[vertBufIdx].addr;
	if (mMapRes.find(pAddr) == mMapRes.end())
		mpVerticies = nullptr;
	else
		mpVerticies = (MyRes_CreateBuffer*)mMapRes[pAddr].first;

	if (mContext.vb[vertBufIdx].numBuf != 1)
		_warn();
	
	MyResBase* pDataLayout = mMapRes[mContext.layout_addr].first;
	((MyRes_CreateLayout*)pDataLayout)->SetNameOffset();
	mLayoutFileID = RES_ID(pDataLayout->crc, pDataLayout->totalSize);

	for (int i = 0; i < 32; ++i)
	{
		void* pAddr = mContext.tex[i].addr;
		if (mMapRes.find(pAddr) != mMapRes.end())
		{
			MyResBase* pDataTex = mMapRes[pAddr].first;
			((MyRes_CreateTexture*)pDataTex)->SetSubResMem();
		}
	}
}
void jParserD3::InitTextureList()
{
	mTextures.clear();
	MyResBase* pData = (MyResBase*)mMapRes[mContext.ps_addr].first;
	int FileID = RES_ID(pData->crc, pData->totalSize);
	switch (FileID)
	{
	case RES_ID(0xF2, 0x1044): //0
	case RES_ID(0x34, 0x2E0): //108
	case RES_ID(0x88, 0x288): //109
	case RES_ID(0x85, 0x1094): //122
	case RES_ID(0x7F, 0x124C): //125
	case RES_ID(0x26, 0x102c): //127
	case RES_ID(0x59, 0xfa4): //130
	case RES_ID(0xb4, 0x14e0): //151
	case RES_ID(0x59, 0x448): //16
	case RES_ID(0xd5, 0x368): //17
	case RES_ID(0xa8, 0x410): //18
	case RES_ID(0xe6, 0x178c): //42
	case RES_ID(0x6d, 0xee4): //72
	case RES_ID(0x58, 0x1428): //75
	case RES_ID(0x30, 0x3a8):	//143
	case RES_ID(0x6f, 0x10e8):	//144
	case RES_ID(0xf3, 0x1554):	//55
	case RES_ID(0x82, 0x14f8):	//56
	case RES_ID(0xec, 0x15b4):	//71
	case RES_ID(0xf4, 0x136c): //69-1
	case RES_ID(0x23, 0xf28):	//135
	case RES_ID(0xe7, 0x4d0):	//152
	case RES_ID(0xd3, 0x494): //14-2
	case RES_ID(0xfb, 0x35c): //17-2
	case RES_ID(0x22, 0x1268): //193-2
	case RES_ID(0x25, 0x130c):
	case RES_ID(0x28, 0x32c):
	case RES_ID(0x42, 0xf3c):
	case RES_ID(0xb3, 0x358):
		mTextures.push_back(0);
		break;
	case RES_ID(0xe3, 0x10e8): //126
	case RES_ID(0x77, 0xff4): //138
	case RES_ID(0x5, 0x424): //150
	case RES_ID(0x39, 0x2198): //21
	case RES_ID(0x32, 0x1468): //34
	case RES_ID(0x75, 0x1418): //38
	case RES_ID(0xe5, 0x1310): //41
	case RES_ID(0x4a, 0x20cc): //82
	case RES_ID(0x3d, 0x1468):	//79
	case RES_ID(0x87, 0x261c): //13-1
	case RES_ID(0x1b, 0x1168): //65-1
	case RES_ID(0xF3, 0x16cc):
	case RES_ID(0x7e, 0x1418):
	case RES_ID(0x5a, 0x1418):
	case RES_ID(0x38, 0x1088):
		mTextures.push_back(1);
		break;
	case RES_ID(0xf4, 0x11a4): //171-2
		mTextures.push_back(2);
		break;
	case RES_ID(0x27, 0x135c): //169-2
	case RES_ID(0xc2, 0x11b0): //167-2
		mTextures.push_back(3);
		break;
	case RES_ID(0xb3, 0x448): //9-2
	case RES_ID(0x6a, 0x102c):	//140, 138-2
		mTextures.push_back(0);
		mTextures.push_back(1);
		break;
	case RES_ID(0x69, 0x16cc):
	case RES_ID(0x8b, 0x188c):
	case RES_ID(0xa3, 0x1290):
		mTextures.push_back(1);
		mTextures.push_back(2);
		break;
	case RES_ID(0xfc, 0x1218): 
	case RES_ID(0x43, 0x117c):
		mTextures.push_back(0);
		mTextures.push_back(1);
		mTextures.push_back(2);
		break;
	case RES_ID(0x64, 0x1290):
		mTextures.push_back(0);
		mTextures.push_back(1);
		mTextures.push_back(2);
		mTextures.push_back(3);
		mTextures.push_back(4);
		break;
	case RES_ID(0x1c, 0x2b70): //19
	case RES_ID(0xaf, 0x2ca4): //20
	case RES_ID(0xfa, 0x2ab8): //84
	case RES_ID(0x7, 0x3094):
	case RES_ID(0xb5, 0x2a40):
		mTextures.push_back(1);
		mTextures.push_back(2);
		mTextures.push_back(3);
		mTextures.push_back(4);
		mTextures.push_back(5);
		mTextures.push_back(6);
		break;

	case RES_ID(0x50, 0x2140): //80 => hard
	case RES_ID(0x32, 0x10a0):	//136
	case RES_ID(0x3d, 0x10e8):	//145
	case RES_ID(0x70, 0x1010):	//157
	case RES_ID(0xc6, 0x12e4):	//184
	case RES_ID(0x18, 0x1348):	//186
	case RES_ID(0x50, 0x330):	//203
	case RES_ID(0x8c, 0x10bc):	//218
	case RES_ID(0x53, 0x10b0):	//219
	case RES_ID(0x8e, 0xfd8):	//220
	case RES_ID(0xbf, 0x1010): //134-1
	case RES_ID(0x34, 0x1180): //139-1
	case RES_ID(0x51, 0x102c): //150-1
	case RES_ID(0xb8, 0x1124): //163-1
	case RES_ID(0x88, 0x127c): //182-2
	case RES_ID(0xbd, 0xf5c): //190-2
	case RES_ID(0x5e, 0x101c): //198-2
	case RES_ID(0xd5, 0x1078): //199-2
	case RES_ID(0x62, 0x11c4): //200-2
	case RES_ID(0xaa, 0x1324): //257-2
	case RES_ID(0x6f, 0xfa8): //261-2
	case RES_ID(0xe4, 0x11fc): //269-2
		_printlog("[ps] default : %d[idx], 0x%x[crc], 0x%x[size]\n", mFileIndex, pData->crc, pData->totalSize);
		mTextures.push_back(0);
		break;
	case RES_ID(0x73, 0x1b58):
	case RES_ID(0x9, 0x1d50):
		_printlog("[ps] skip too hard... : %d[idx], 0x%x[crc], 0x%x[size]\n", mFileIndex, pData->crc, pData->totalSize);
		break;
	default:
		_printlog("[ps] undefine : %d[idx], 0x%x[crc], 0x%x[size]\n", mFileIndex, pData->crc, pData->totalSize);
		break;
	}
}
void jParserD3::InitCBMain()
{
	stringstream ss;
	ss << mFileIndex << "_" << mContext.CBMain.addr << "_b.dump";

	int fileSize = 0;
	MyRes_CreateBuffer* pData = nullptr;
	jUtils::LoadFile(PATH_PARSER_DATA + ss.str(), &fileSize, (char**)&pData);
	memcpy(&mCBMain, pData->data, sizeof(mCBMain));
	free(pData);
}
bool jParserD3::IsValid()
{
	if( mpVerticies == nullptr )
		return false;
	if (mLayoutFileID == 0)
		return false;
	if (mFuncConvertTex == nullptr)
		return false;

	return true;
}

void jParserD3::InitAnims()
{
	stringstream ss;
	ss << mFileIndex << "_" << mContext.tex[0].addr << "_c";

	int fileSize = 0;
	MyRes_CreateAnimations* pData = nullptr;
	jUtils::LoadFile(PATH_PARSER_DATA + ss.str() + ".dump", &fileSize, (char**)&pData);
	
	if (pData)
	{
		mAnimFileName = ss.str();
		int i = 0;
		while (pData->offset[i] != 0)
		{
			int off = pData->offset[i];
			int count = pData->counts[i];
			Matrix3x4f* pMat = (Matrix3x4f*)((char*)pData + off);
			mAnims.push_back(AnimInfo());
			for (int j = 0; j < count; ++j)
			{
				AnimInfo& info = mAnims.back();
				
				vector<Matrix4f> key;
				for (int k = 0; k < 45; ++k)
					key.push_back(pMat[k + j * 45].ToMat());

				info.keys.push_back(key);
			}
			
			i++;
		}
	}
	
	free(pData);
}


void* jParserD3::CreateD3DRescource(void* addr)
{
	if (mMapRes.find(addr) == mMapRes.end())
		return nullptr;

	MyResBase* pData = mMapRes[addr].first;
	void* pIF = mMapRes[addr].second;
	if (pIF != nullptr)
		return pIF;

	switch (pData->type)
	{
	case MYRES_TYPE_D3D9_IB:
		pIF = ((MyRes_D3D9_IB*)pData)->CreateResource();
		break;
	case MYRES_TYPE_CreateBuffer:
		pIF = ((MyRes_CreateBuffer*)pData)->CreateResource();
		break;
	case MYRES_TYPE_CreateLayout:
	{
		MyRes_CreateShader* pShaderData = (MyRes_CreateShader*)mMapRes[mContext.vs_addr].first;
		int size = pShaderData->head.totalSize - sizeof(MyRes_CreateShader);
		void *bolb = pShaderData->data;
		pIF = ((MyRes_CreateLayout*)pData)->CreateResource(bolb, size);
	}
	break;
	case MYRES_TYPE_CreatePS:
	case MYRES_TYPE_CreateVS:
		pIF = ((MyRes_CreateShader*)pData)->CreateResource();
		break;
	case MYRES_TYPE_CreateSample:
		pIF = ((MyRes_CreateSS*)pData)->CreateResource();
		break;
	case MYRES_TYPE_CreateBlend:
		pIF = ((MyRes_CreateBS*)pData)->CreateResource();
		break;
	case MYRES_TYPE_CreateDapth:
		pIF = ((MyRes_CreateDS*)pData)->CreateResource();
		break;
	case MYRES_TYPE_CreateTex:
	{
		stringstream ss;
		ss << PATH_PARSER_DATA << "*_" << addr << "_t.tga";

		int width = 0;
		int height = 0;
		chars imgbuf;
		jUtils::ForEachFiles2(nullptr, ss.str().c_str(), [&](void *_obj, string _filename) {
			imgbuf = jUtils::LoadTarga(PATH_PARSER_DATA + _filename, height, width);
			return false;
		});

		if(imgbuf)
			pIF = ((MyRes_CreateTexture*)pData)->CreateResource(width, height, &imgbuf[0]);
	}
	break;
	default:
		break;
	}
	mMapRes[addr].second = pIF;
	return pIF;
}
ID3D11Buffer* jParserD3::GetResIndexBuffer()
{
	return (ID3D11Buffer*)CreateD3DRescource(mContext.ib_addr);
}
ID3D11BlendState * jParserD3::GetResBlendState()
{
	if (mMapRes.find(mContext.bs_addr) == mMapRes.end())
		return nullptr;

	return (ID3D11BlendState *)CreateD3DRescource(mContext.bs_addr);
}
ID3D11DepthStencilState * jParserD3::GetResDepthStencilState()
{
	if (mMapRes.find(mContext.ds_addr) == mMapRes.end())
		return nullptr;

	return (ID3D11DepthStencilState *)CreateD3DRescource(mContext.ds_addr);
}
ID3D11ShaderResourceView* jParserD3::GetResShaderResourceView(int _idx)
{
	if (mTextures.size() == 0)
		return nullptr;

	int idx = mTextures[_idx];
	return (ID3D11ShaderResourceView *)CreateD3DRescource(mContext.tex[idx].addr);
}
bool jParserD3::SetContantBuffer(MapInfo & cb, int slotIdx)
{
	auto pDevContext = jRenderer::GetInst().GetDeviceContext();
	ID3D11Buffer *pConstBuf = (ID3D11Buffer *)CreateD3DRescource(cb.addr);
	if (pConstBuf == nullptr)
		return false;

	stringstream ss;
	ss << mFileIndex << "_" << cb.addr << "_b.dump";

	int fileSize = 0;
	MyRes_CreateBuffer* pConstData = nullptr;
	jUtils::LoadFile(PATH_PARSER_DATA + ss.str(), &fileSize, (char**)&pConstData);
	if (pConstData == nullptr)
		return false;

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if (FAILED(pDevContext->Map(pConstBuf, cb.subRes, (D3D11_MAP)cb.type, cb.flags, &mappedRes)))
	{
		_warn();
		return false;
	}
	memcpy(mappedRes.pData, pConstData->data, pConstData->head.totalSize - sizeof(MyRes_CreateBuffer));
	pDevContext->Unmap(pConstBuf, cb.subRes);

	pDevContext->VSSetConstantBuffers(slotIdx, 1, &pConstBuf);
	pDevContext->PSSetConstantBuffers(slotIdx, 1, &pConstBuf);

	free(pConstData);

	return true;
}

bool jParserD3::Render()
{
	auto pDevContext = jRenderer::GetInst().GetDeviceContext();
	for (int i = 0; i < 32; ++i)
	{
		VBInfo& vb = mContext.vb[i];
		ID3D11Buffer *pVertBuf = (ID3D11Buffer *)CreateD3DRescource(vb.addr);
		if (pVertBuf != nullptr)
			pDevContext->IASetVertexBuffers(i, vb.numBuf, &pVertBuf, vb.strides, vb.offset);
	}

	ID3D11Buffer *pIdxBuf = (ID3D11Buffer *)CreateD3DRescource(mContext.ib_addr);
	if (pIdxBuf != nullptr)
		pDevContext->IASetIndexBuffer(pIdxBuf, (DXGI_FORMAT)mContext.ib_format, mContext.ib_offset);
	
	pDevContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)mContext.prim_topology);

	SetContantBuffer(mContext.CBMain, 0);
	SetContantBuffer(mContext.CBBones, 1);
	SetContantBuffer(mContext.CBLights, 2);

	for (int i = 0; i < 32; ++i)
	{
		if (i == 4)
			continue;

		TEXInfo& tb = mContext.tex[i];
		ID3D11ShaderResourceView *pTexBuf = (ID3D11ShaderResourceView *)CreateD3DRescource(tb.addr);
		if (pTexBuf != nullptr)
			pDevContext->PSSetShaderResources(i, tb.NumViews, &pTexBuf);
	}

	ID3D11InputLayout *pLayoutIF = (ID3D11InputLayout *)CreateD3DRescource(mContext.layout_addr);
	if (pLayoutIF != nullptr)
		pDevContext->IASetInputLayout(pLayoutIF);

	ID3D11VertexShader *pVertexShaderIF = (ID3D11VertexShader *)CreateD3DRescource(mContext.vs_addr);
	if (pVertexShaderIF != nullptr)
		pDevContext->VSSetShader(pVertexShaderIF, NULL, 0);
	ID3D11PixelShader *pPixelShaderIF = (ID3D11PixelShader *)CreateD3DRescource(mContext.ps_addr);
	if (pPixelShaderIF != nullptr)
		pDevContext->PSSetShader(pPixelShaderIF, NULL, 0);

	ID3D11BlendState *pBS = (ID3D11BlendState *)CreateD3DRescource(mContext.bs_addr);
	if (pBS != nullptr)
		pDevContext->OMSetBlendState(pBS, mContext.bs_factor, mContext.bs_mask);
	
	ID3D11DepthStencilState *pDSS = (ID3D11DepthStencilState *)CreateD3DRescource(mContext.ds_addr);
	if (pDSS != nullptr)
		pDevContext->OMSetDepthStencilState(pDSS, mContext.ds_ref);

	for (int i = 0; i < 32; ++i)
	{
		SSInfo& ss = mContext.ss[i];
		ID3D11SamplerState *pSS = (ID3D11SamplerState *)CreateD3DRescource(ss.addr);
		if (pSS != nullptr)
			pDevContext->PSSetSamplers(i, ss.NumSamplers, &pSS);
	}

	pDevContext->DrawIndexed(mContext.draw_IndexCount, mContext.draw_StartIndex, mContext.draw_BaseVertex);

	return true;
}


Vector3f jParserD3::GetPos(int _idx, int byteOffset)
{
	_warnif(_idx >= mGeoInfo.vertexTotalCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	char* pStreamOff = pData->data + byteOffset;
	switch (mLayoutFileID)
	{
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pStreamOff;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x2f, 0x15f):
	case RES_ID(0xe9, 0x10e):
	{
		LayoutB* pVert = (LayoutB*)pStreamOff;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x83, 0x18b):
	{
		LayoutC* pVert = (LayoutC*)pStreamOff;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0xca, 0x110):
	{
		LayoutD* pVert = (LayoutD*)pStreamOff;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x1d, 0x137):
	{
		LayoutE* pVert = (LayoutE*)pStreamOff;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0xd2, 0xe5):
	{
		LayoutF* pVert = (LayoutF*)pStreamOff;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	default:
		_warn();
		break;
	}
	return Vector3f();
}
Vector3f jParserD3::GetNor(int _idx, int byteOffset)
{
	_warnif(_idx >= mGeoInfo.vertexTotalCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	char* pStreamOff = pData->data + byteOffset;
	switch (mLayoutFileID)
	{
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pStreamOff;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	case RES_ID(0x2f, 0x15f):
	case RES_ID(0xe9, 0x10e):
	{
		LayoutB* pVert = (LayoutB*)pStreamOff;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	case RES_ID(0x83, 0x18b):
	{
		return Vector3f(0,1,0);
	}
	case RES_ID(0xca, 0x110):
	{
		return Vector3f(0, 1, 0);
	}
	case RES_ID(0x1d, 0x137):
	{
		LayoutE* pVert = (LayoutE*)pStreamOff;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	case RES_ID(0xd2, 0xe5):
	{
		LayoutF* pVert = (LayoutF*)pStreamOff;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	default:
		_warn();
		break;
	}
	return Vector3f(0, 1, 0);
}
int jParserD3::GetTex(int _idx, Vector2f* _t, int byteOffset)
{
	_warnif(_idx >= mGeoInfo.vertexTotalCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	char* pStreamOff = pData->data + byteOffset;
	switch (mLayoutFileID)
	{
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pStreamOff;
		_t[0] = mFuncConvertTex(0, pVert[_idx].t0);
		return 1;
	}
	case RES_ID(0xe9, 0x10e):
	{
		LayoutB* pVert = (LayoutB*)pStreamOff;
		int cnt = mTextures.size();
		for (int i = 0; i < cnt; ++i)
			_t[i] = mFuncConvertTex(i, pVert[_idx].t0);
		return cnt;
	}
	case RES_ID(0x83, 0x18b):
	{
		LayoutC* pVert = (LayoutC*)pStreamOff;
		int cnt = mTextures.size();
		for (int i = 0; i < cnt; ++i)
			_t[i] = mFuncConvertTex(i, pVert[_idx].t0 + 4*i );
		return cnt;
	}
	case RES_ID(0xca, 0x110):
	{
		LayoutD* pVert = (LayoutD*)pStreamOff;
		_t[0] = mFuncConvertTex(0, (unsigned char*)pVert[_idx].t0);
		return 1;
	}
	case RES_ID(0x2f, 0x15f):
	{
		LayoutB* pVert = (LayoutB*)pStreamOff;
		int cnt = mTextures.size();
		for (int i = 0; i < cnt; ++i)
			_t[i] = mFuncConvertTex(i, pVert[_idx].t0);
		return cnt;
	}
	case RES_ID(0x1d, 0x137):
	{
		LayoutE* pVert = (LayoutE*)pStreamOff;
		_t[0] = mFuncConvertTex(0, (unsigned char*)pVert[_idx].t0);
		return 1;
	}
	case RES_ID(0xd2, 0xe5):
	{
		LayoutF* pVert = (LayoutF*)pStreamOff;
		int cnt = mTextures.size();
		for (int i = 0; i < cnt; ++i)
			_t[i] = mFuncConvertTex(i, pVert[_idx].t0);
		return cnt;
	}
	default:
		_warn();
		break;
	}
	return 0;
}
Vector4n jParserD3::GetMatIdx(int _idx, int byteOffset)
{
	_warnif(_idx >= mGeoInfo.vertexTotalCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	switch (mLayoutFileID)
	{
	//case RES_ID(0x59, 0x167):
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)(pData->data + byteOffset);
		Vector4n ret = Vector4n(pVert[_idx].i[0], pVert[_idx].i[1], pVert[_idx].i[2], pVert[_idx].i[3]);
		return ret;
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	default:
		//_warn();
		break;
	}
	return Vector4n(0, 0, 0, 0);
}
Vector4f jParserD3::GetMatWeight(int _idx, int byteOffset)
{
	_warnif(_idx >= mGeoInfo.vertexTotalCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	switch (mLayoutFileID)
	{
	//case RES_ID(0x59, 0x167):
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)(pData->data + byteOffset);
		float other = 1 - (pVert[_idx].w[0] + pVert[_idx].w[1] + pVert[_idx].w[2]);
		Vector4f ret = Vector4f(pVert[_idx].w[0], pVert[_idx].w[1], pVert[_idx].w[2], other);
		return ret;
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	default:
		//_warn();
		break;
	}
	return Vector4f(1, 0, 0, 0);
}
char * jParserD3::GetIndiciesData()
{
	void* ibAddr = mContext.ib_addr;
	MyRes_CreateBuffer* pDataIB = (MyRes_CreateBuffer*)mMapRes[ibAddr].first;
	return pDataIB->data;
}

void * jParserD3::GetTexResAddr(int _idx)
{
	if (_idx >= mTextures.size())
		return nullptr;

	return mContext.tex[mTextures[_idx]].addr;
}

bool jParserD3::IsTerrain()
{
	float posX = mCBMain.matWorld[3];
	if (!COMP_FLOAT(posX, (int)posX))
		return false;
	float posY = mCBMain.matWorld[7];
	if (!COMP_FLOAT(posY, (int)posY))
		return false;
	float posZ = mCBMain.matWorld[11];
	if (!COMP_FLOAT(posZ, (int)posZ))
		return false;

	if (posX == 0 && posY == 0 && posZ == 0)
		return false;

	float axisX = mCBMain.matWorld[0];
	if (!COMP_FLOAT(axisX, 1.0f))
		return false;
	float axisY = mCBMain.matWorld[5];
	if (!COMP_FLOAT(axisY, 1.0f))
		return false;
	float axisZ = mCBMain.matWorld[10];
	if (!COMP_FLOAT(axisZ, 1.0f))
		return false;

	return true;
}
unsigned long long jParserD3::CalcKey(float _x, float _y)
{
	UINT64 key = 0;
	memcpy(&key, &_x, 4);
	memcpy((char*)&key + 4, &_y, 4);
	return key;
}
bool jParserD3::ExportToObjectFormat(string type, bool alpha, bool depth)
{
	mExportInfo.Reset();
	char* pIndiciesData = GetIndiciesData();

	int vertCount = mGeoInfo.vertexTotalCount;
	int vertByteOffset = mGeoInfo.vertexVertexByteOffset + (mGeoInfo.vertexStride * mGeoInfo.drawVertOffset);
	int polyCount = mGeoInfo.drawIndexCount / mGeoInfo.indiciesCntPerPoly;
	int indexOff = mGeoInfo.drawIndexOffset;

	mExportInfo.name = "MyObject_" + to_string(mFileIndex);
	stringstream ss;
	//ss << mContext.vb[0].addr << "_" << mFileIndex << ".obj";
	ss << mContext.vb[0].addr << "_" << mFileIndex << ".dump";
	mExportInfo.objname = ss.str();
	mExportInfo.type = type;
	mExportInfo.alpha = alpha;
	mExportInfo.depth = depth;
	short* pIndicies = (short*)pIndiciesData + indexOff;
	for (int i = 0; i < polyCount; ++i)
	{
		Vector3n index;
		index.x = pIndicies[i * 3 + 0];
		index.y = pIndicies[i * 3 + 1];
		index.z = pIndicies[i * 3 + 2];

		AddVertInfo(index.x, vertByteOffset);
		AddVertInfo(index.y, vertByteOffset);
		AddVertInfo(index.z, vertByteOffset);

		mExportInfo.indicies.push_back(index);
	}

	Vector3f basePos;
	basePos.x = mCBMain.matWorld[3];
	basePos.y = mCBMain.matWorld[7];
	basePos.z = mCBMain.matWorld[11];

	mExportInfo.metaInfo.worldPosition = basePos;
	int cntTextures = mTextures.size();
	for (int i = 0; i < cntTextures; ++i)
	{
		int idx = mTextures[i];
		stringstream ss;
		void* pTexAddr = GetTexResAddr(i);
		//ss << "*_" << pTexAddr << "_*.tga";
		ss << "*_" << pTexAddr << "_*.dump";
		string imageName = jUtils::FindFile(PATH_PARSER_DATA, ss.str());
		mExportInfo.metaInfo.images.push_back(imageName);

		Matrix4f& mat = mCBMain.matTex[i + 1];
		mExportInfo.metaInfo.vectors.push_back(Vector3f(mat[0], mat[1], mat[3]));
		mExportInfo.metaInfo.vectors.push_back(Vector3f(mat[4], mat[5], mat[7]));
	}

	if (mAnims.size() > 0)
	{
		mExportInfo.animName = mAnimFileName + ".csv";
		for (int i = 0; i < mAnims[0].keys.size(); ++i)
		{
			string line = "";
			int cnt = mAnims[0].keys[i].size();
			for (int j = 0; j < cnt; ++j)
			{
				Matrix4f& mat = mAnims[0].keys[i][j];
				line += jUtils::MatToCSV(&mat) + ",";
			}
			mExportInfo.animationInfo.push_back(line);
		}
	}
	else
		mExportInfo.animName = "NoAnim";

	mExportInfo.ExportAnimInfo(PATH_RESOURCES + string("anim/"));
	mExportInfo.ExportMetaInfo(PATH_RESOURCES + string("meta/"));
	mExportInfo.ExportImage(PATH_RESOURCES + string("img/"));
	mExportInfo.ExportMesh(PATH_RESOURCES + string("mesh/"), true, 0);
	
	//if (IsTerrain())
	//{
	//	unsigned long long key = CalcKey(mCBMain.matWorld[3], mCBMain.matWorld[7]);
	//	if (mExpLinks.find(key) == mExpLinks.end())
	//		mExpLinks[key] = &mExportInfo;
	//	else
	//		mExpLinks[key]->Merge(&mExportInfo);
	//}
	//else
	//	mExportInfo.ExportToObject(mExportInfo.name + ".obj", true, 0);

	return true;
}
bool jParserD3::AddVertInfo(int index, int offset)
{
	Vector2f texel[10];
	ExpVertex vertex;
	vertex.p = GetPos(index, offset);
	vertex.n = GetNor(index, offset);
	GetTex(index, texel, offset);
	vertex.t = texel[0];
	vertex.w = GetMatWeight(index, offset);
	vertex.i = GetMatIdx(index, offset);

	mExportInfo.vert.push_back(vertex);
	return true;
}
bool ExpMesh::ExportMesh(string _path, bool _isRoot, int _baseIdx)
{
	string fullname = _path + objname;
	if (jUtils::ExistFile(fullname))
		return true;

	if (jUtils::GetFileExtension(objname) == "dump")
	{
		ExportMeshDump(_path);
		return true;
	}

	string strBaseIdx = to_string(_baseIdx);
	string ret = "";
	if (_isRoot)
		ret = "o " + name + "\n";

	ret += ("g " + name + "\n");

	string pos = "";
	string nor = "";
	string tex = "";
	string wei = "";
	string weiIdx = "";
	char tmpBuf[64] = { 0, };
	int vertCount = vert.size();
	for (int i = 0; i < vertCount; ++i)
	{
		ExpVertex& curVert = vert[i];
		memset(tmpBuf, 0x00, 64);
		sprintf_s(tmpBuf, "v %.4f %.4f %.4f\n", curVert.p.x, curVert.p.y, curVert.p.z);
		pos += tmpBuf;
		memset(tmpBuf, 0x00, 64);
		sprintf_s(tmpBuf, "vn %.4f %.4f %.4f\n", curVert.n.x, curVert.n.y, curVert.n.z);
		nor += tmpBuf;
		memset(tmpBuf, 0x00, 64);
		sprintf_s(tmpBuf, "vt %.4f %.4f %.4f\n", curVert.t.x, 1 - curVert.t.y, 0.0f);
		tex += tmpBuf;
		memset(tmpBuf, 0x00, 64);
		sprintf_s(tmpBuf, "w %.4f %.4f %.4f %.4f\n", curVert.w.x, curVert.w.y, curVert.w.z, curVert.w.w);
		wei += tmpBuf;
		memset(tmpBuf, 0x00, 64);
		sprintf_s(tmpBuf, "i %d %d %d %d\n", curVert.i.x, curVert.i.y, curVert.i.z, curVert.i.w);
		weiIdx += tmpBuf;
	}

	string strIndicies = "";
	int indiCount = indicies.size();
	for (int i = 0; i < indiCount; ++i)
	{
		Vector3n& index = indicies[i];
		int xIdx = _baseIdx + index.x + 1;
		int yIdx = _baseIdx + index.y + 1;
		int zIdx = _baseIdx + index.z + 1;

		memset(tmpBuf, 0x00, 64);
		sprintf_s(tmpBuf, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			xIdx, xIdx, xIdx,
			yIdx, yIdx, yIdx,
			zIdx, zIdx, zIdx );
		strIndicies += tmpBuf;
	}

	ret += pos;
	ret += nor;
	ret += tex;
	ret += wei;
	ret += weiIdx;
	ret += strIndicies;

	jUtils::SaveToFile(_path, objname, ret, true);

	if (pNext != nullptr)
		pNext->ExportMesh(_path, false, _baseIdx + vertCount);

	return true;
}
bool ExpMesh::ExportMeshDump(string _path)
{
	int size = 0;
	char* data = nullptr;
	if (type == "terrain")
	{
		int cnt = vert.size();
		size = sizeof(VertexFormatPT) * cnt;
		data = (char*)malloc(size);
		memset(data, 0x00, size);
		VertexFormatPT* pVert = (VertexFormatPT*)data;
		for (int i = 0; i < cnt; ++i)
		{
			pVert[i].p = vert[i].p;
			pVert[i].t = vert[i].t;
		}
	}
	else if (type == "skin")
	{
		int cnt = vert.size();
		size = sizeof(VertexFormatPTNIW) * cnt;
		data = (char*)malloc(size);
		memset(data, 0x00, size);
		VertexFormatPTNIW* pVert = (VertexFormatPTNIW*)data;
		for (int i = 0; i < cnt; ++i)
		{
			pVert[i].p = vert[i].p;
			pVert[i].t = vert[i].t;
			pVert[i].n = vert[i].n;
			pVert[i].i = vert[i].i;
			pVert[i].w = vert[i].w;
		}
	}
	else if (type == "default")
	{
		int cnt = vert.size();
		size = sizeof(VertexFormatPTN) * cnt;
		data = (char*)malloc(size);
		memset(data, 0x00, size);
		VertexFormatPTN* pVert = (VertexFormatPTN*)data;
		for (int i = 0; i < cnt; ++i)
		{
			pVert[i].p = vert[i].p;
			pVert[i].t = vert[i].t;
			pVert[i].n = vert[i].n;
		}
	}

	jUtils::SaveToFile(_path, objname, data, size);

	free(data);
	return false;
}
bool ExpMesh::ExportAnimInfo(string path)
{
	if(animationInfo.size() <= 0)
		return false;

	string total = "";
	for (string line : animationInfo)
	{
		total += line + "\n";
	}
	jUtils::SaveToFile(path, animName, total);

	return true;
}
bool ExpMesh::ExportMetaInfo(string path)
{
	string folderPath = "";
	if (type == "skin")
	{
		folderPath = path;
	}
	else
	{
		int blockPosX = (int)(metaInfo.worldPosition.x / 240) * 240;
		int blockPosY = (int)(metaInfo.worldPosition.y / 240) * 240;
		folderPath = path + to_string(blockPosX) + "_" + to_string(blockPosY);
		_mkdir(folderPath.c_str());
	}
	

	string ret = objname + "\n";
	ret += type + "\n";
	ret += alpha ? "TRUE\n" : "FALSE\n";
	ret += depth ? "TRUE\n" : "FALSE\n";
	ret += to_string(metaInfo.worldPosition.x) + " ";
	ret += to_string(metaInfo.worldPosition.y) + " ";
	ret += to_string(metaInfo.worldPosition.z) + "\n";
	ret += animName + "\n";
	int cnt = metaInfo.vectors.size();
	for (int i = 0; i < cnt; ++i)
	{
		ret += to_string(metaInfo.vectors[i].x) + " ";
		ret += to_string(metaInfo.vectors[i].y) + " ";
		ret += to_string(metaInfo.vectors[i].z) + "\n";
	}

	cnt = metaInfo.images.size();
	for (int i = 0; i < cnt; ++i)
		ret += metaInfo.images[i] + "\n";

	jUtils::SaveToFile(folderPath, name + ".txt", ret);
	
	return true;
}
bool ExpMesh::ExportImage(string path)
{
	int cnt = metaInfo.images.size();
	for (int i = 0; i < cnt; ++i)
	{
		string imageName = metaInfo.images[i];
		if(jUtils::ExistFile(imageName) == false)
			jUtils::MyCopyFile(PATH_PARSER_DATA + imageName, path + imageName);
	}
	return true;
}
bool ExpMesh::Merge(ExpMesh * _mesh)
{
	ExpMesh **ppMesh = &pNext;
	while (*ppMesh != nullptr)
		ppMesh = &(*ppMesh)->pNext;

	*ppMesh = _mesh;
	return true;
}
