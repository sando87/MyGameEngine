#include "jParserD3.h"
#include "jUtils.h"
#include "jLog.h"
#include <sstream>

#define REV_V(v) (1.0f - (v))
#define RES_ID(crc, size) (((size)<<8) | (crc))

map<void*, pair<MyResBase*, void*>> jParserD3::mMapRes;

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
	mMapRes.clear();
}


jParserD3::~jParserD3()
{
}


void jParserD3::LoadResources()
{
	string path = PATH_RESOURCE;
	string filter = "*.dump";
	jUtils::ForEachFiles2(nullptr, (path + filter).c_str(), [&](void* _obj, string _str) {

		vector<string> rets;
		jUtils::Split(_str, "_.", rets);
		unsigned long long nAddr = 0;
		stringstream ss;
		ss << std::hex << rets[1];
		ss >> nAddr;
		void* id = (void*)nAddr;
		if (mMapRes.find(id) != mMapRes.end())
			return true;

		int filesize = 0;
		MyResBase* pBuf = nullptr;
		bool ret = jUtils::LoadFile(path + _str, &filesize, (char**)&pBuf);
		if (!ret || pBuf == nullptr)
		{
			_warn();
			return true;
		}

		//if (pBuf->type == MYRES_TYPE_CreateBuffer)
		//{
		//	int idx = stoi(rets[0]);
		//	if (((MyRes_CreateBuffer*)pBuf)->desc.BindFlags == D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER &&
		//		((MyRes_CreateBuffer*)pBuf)->desc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		//		printf(".....%d....%p....\n", idx, id);
		//}

		mMapRes[id] = make_pair(pBuf, nullptr);
		return true;
	});


	//path = PATH_RESOURCE;
	//filter = "*.bin";
	//jUtils::ForEachFiles2(nullptr, (path + filter).c_str(), [&](void* _obj, string _str) {
	//
	//	vector<string> rets;
	//	jUtils::Split(_str, "_.", rets);
	//
	//	int filesize = 0;
	//	RenderContext* pBuf = nullptr;
	//	jUtils::LoadFile(path + _str, &filesize, (char**)&pBuf);
	//	MyResBase* pData = mMapRes[pBuf->vs_addr].first;
	//	if(pBuf->ds_isDirty)
	//		printf(".....%d....\n", stoi(rets[0]));
	//
	//	return true;
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
	LoadResources();

	mFileIndex = _fileIdx;
	string name = PATH_RESOURCE + to_string(_fileIdx) + "_RenderingContext.bin";
	int size = 0;
	char* pBuf = nullptr;
	jUtils::LoadFile(name, &size, (char**)&pBuf);
	memcpy(&mContext, pBuf, sizeof(mContext));

	InitCBMain();
	ReadyForData();
	InitFuncConvTex();
	InitTextureList();

	if (!IsValid())
	{
		_warn();
		return false;
	}

	return true;
}
void jParserD3::InitFuncConvTex()
{
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
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[_idx];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
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
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[1];
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
	case RES_ID(0x84, 0x1A30): //19 
	case RES_ID(0xed, 0x1b8c):	//20 
	case RES_ID(0x51, 0x1a30):	//84 
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4f matTex = mCBMain.matTex[_idx + 1];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		break;
	case RES_ID(0x28, 0x121c):	//152
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			float* p = (float*)_p;
			Matrix4f matTex = mCBMain.matTex[0];
			Vector2f ret;
			ret.x = p[0] * matTex[0] + p[1] * matTex[1];
			ret.y = p[0] * matTex[4] + p[1] * matTex[5];
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

	case RES_ID(0x92, 0x1378):	//90
		_printlog("[vs] texel is created on view of camera and dynamic\n");
	case RES_ID(0xd1, 0x2150):	//80 => hard
	case RES_ID(0x2c, 0x1240):	//157
	case RES_ID(0x8e, 0x1168):	//159
	case RES_ID(0x85, 0x150c):	//184
	case RES_ID(0x3a, 0x1564):	//186
	case RES_ID(0xe3, 0x11dc):	//191
	case RES_ID(0xff, 0xfcc):		//220
	case RES_ID(0xc5, 0x18b0): //130
	case RES_ID(0xc1, 0x1518): //131
	case RES_ID(0xd6, 0x1e50): //139
		mFuncConvertTex = [&](int _idx, unsigned char* _p) {
			Matrix4 matTex = mCBMain.matTex[_idx];
			Vector2f tmp = CalcTexCoord(_p);
			Vector2f ret;
			ret.x = tmp.x * matTex[0] + tmp.y * matTex[1] + (1.0f) * matTex[3];
			ret.y = tmp.x * matTex[4] + tmp.y * matTex[5] + (1.0f) * matTex[7];
			return Vector2f(ret.x, REV_V(ret.y));
		};
		_printlog("[vs] skipped\n");
		break;
	default:
		_warn();
		mFuncConvertTex = nullptr;
		break;
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

	if (mpVerticies != nullptr)
	{
		int vertexBufSize = mpVerticies->head.totalSize - sizeof(MyRes_CreateBuffer);
		mVertexStride = mContext.vb[vertBufIdx].strides[0];
		mVertexOffset = mContext.vb[vertBufIdx].offset[0];
		mVertexCount = vertexBufSize / mVertexStride;
	}

	MyResBase* pDataLayout = mMapRes[mContext.layout_addr].first;
	((MyRes_CreateLayout*)pDataLayout)->SetNameOffset();
	mLayoutFileID = RES_ID(pDataLayout->crc, pDataLayout->totalSize);
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
	case RES_ID(0xfc, 0x1218): //139-2
		mTextures.push_back(0);
		mTextures.push_back(1);
		mTextures.push_back(2);
		break;
	case RES_ID(0x1c, 0x2b70): //19
	case RES_ID(0xaf, 0x2ca4): //20
	case RES_ID(0xfa, 0x2ab8): //84
		mTextures.push_back(1);
		mTextures.push_back(2);
		mTextures.push_back(3);
		mTextures.push_back(4);
		mTextures.push_back(5);
		break;

	case RES_ID(0x50, 0x2140): //80 => hard
	case RES_ID(0x25, 0x130c): //90 => skip
	case RES_ID(0x5a, 0x1418):  //124
	case RES_ID(0x32, 0x10a0):	//136
	case RES_ID(0x3d, 0x10e8):	//145
	case RES_ID(0x70, 0x1010):	//157
	case RES_ID(0x42, 0xf3c):	//162
	case RES_ID(0xc6, 0x12e4):	//184
	case RES_ID(0x64, 0x1290):	//185
	case RES_ID(0x18, 0x1348):	//186
	case RES_ID(0x50, 0x330):	//203
	case RES_ID(0xb3, 0x358):	//204
	case RES_ID(0x8c, 0x10bc):	//218
	case RES_ID(0x53, 0x10b0):	//219
	case RES_ID(0x8e, 0xfd8):	//220
	case RES_ID(0x38, 0x1088): //131-1
	case RES_ID(0xbf, 0x1010): //134-1
	case RES_ID(0x34, 0x1180): //139-1
	case RES_ID(0x51, 0x102c): //150-1
	case RES_ID(0x28, 0x32c): //161-1
	case RES_ID(0xb8, 0x1124): //163-1
	case RES_ID(0x88, 0x127c): //182-2
	case RES_ID(0xbd, 0xf5c): //190-2
	case RES_ID(0x5e, 0x101c): //198-2
	case RES_ID(0xd5, 0x1078): //199-2
	case RES_ID(0x62, 0x11c4): //200-2
	case RES_ID(0xaa, 0x1324): //257-2
	case RES_ID(0x6f, 0xfa8): //261-2
	case RES_ID(0xe4, 0x11fc): //269-2
		mTextures.push_back(0);
		_printlog("[ps] skipped\n");
		break;
	default:
		_warn();
		break;
	}
}
void jParserD3::InitCBMain()
{
	CBMain ret;
	for (int i = 0; i < 8; ++i)
	{
		stringstream ss;
		ss << mFileIndex << "_" << mContext.mapUnmap[i].addr << "_b.dump";

		int fileSize = 0;
		MyRes_CreateBuffer* pData = nullptr;
		jUtils::LoadFile(PATH_RESOURCE + ss.str(), &fileSize, (char**)&pData);
		if (pData->head.totalSize == sizeof(MyRes_CreateBuffer) + sizeof(CBMain))
		{
			memcpy(&ret, pData->data, sizeof(ret));
			free(pData);
			break;
		}
		free(pData);
	}
	mCBMain = ret;
}
bool jParserD3::IsValid()
{
	if( mpVerticies == nullptr )
		return false;
	if (mLayoutFileID == 0)
		return false;
	if (mTextures.size() == 0)
		return false;
	if (mFuncConvertTex == nullptr)
		return false;

	return true;
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
	case MYRES_TYPE_CreateTex:
	{
		stringstream ss;
		ss << PATH_RESOURCE << "*_" << addr << "_t.dump.tga";

		int width = 0;
		int height = 0;
		int bufSize = 0;
		unsigned char* imgbuf = nullptr;
		jUtils::ForEachFiles2(nullptr, ss.str().c_str(), [&](void *_obj, string _filename) {
			if (!jUtils::LoadTarga(PATH_RESOURCE + _filename, height, width, bufSize, imgbuf))
				_warn();
			return false;
		});

		if (imgbuf != nullptr)
		{
			pIF = ((MyRes_CreateTexture*)pData)->CreateResource(width, height, (char*)imgbuf);
			delete[] imgbuf;
		}
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
ID3D11ShaderResourceView* jParserD3::GetResShaderResourceView()
{
	if (mTextures.size() == 0)
		return nullptr;

	int idx = mTextures[0];
	return (ID3D11ShaderResourceView *)CreateD3DRescource(mContext.tex[idx].addr);
}


Vector3f jParserD3::GetPos(int _idx)
{
	_warnif(_idx >= mVertexCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	switch (mLayoutFileID)
	{
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pData->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x2f, 0x15f):
	case RES_ID(0xe9, 0x10e):
	{
		LayoutB* pVert = (LayoutB*)pData->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x83, 0x18b):
	{
		LayoutC* pVert = (LayoutC*)mpVerticies->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0xca, 0x110):
	{
		LayoutD* pVert = (LayoutD*)mpVerticies->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x1d, 0x137):
	{
		LayoutE* pVert = (LayoutE*)mpVerticies->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0xd2, 0xe5):
	{
		LayoutF* pVert = (LayoutF*)mpVerticies->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	default:
		_warn();
		break;
	}
	return Vector3f();
}
Vector3f jParserD3::GetNor(int _idx)
{
	_warnif(_idx >= mVertexCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	switch (mLayoutFileID)
	{
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pData->data;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	case RES_ID(0x2f, 0x15f):
	case RES_ID(0xe9, 0x10e):
	{
		LayoutB* pVert = (LayoutB*)pData->data;
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
		LayoutE* pVert = (LayoutE*)mpVerticies->data;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	case RES_ID(0xd2, 0xe5):
	{
		LayoutF* pVert = (LayoutF*)mpVerticies->data;
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
int jParserD3::GetTex(int _idx, Vector2f* _t)
{
	_warnif(_idx >= mVertexCount);

	switch (mLayoutFileID)
	{
	case RES_ID(0x3e, 0x167):
	{
		LayoutA* pVert = (LayoutA*)mpVerticies->data;
		_t[0] = mFuncConvertTex(0, pVert[_idx].t0);
		return 1;
	}
	case RES_ID(0xe9, 0x10e):
	{
		LayoutB* pVert = (LayoutB*)mpVerticies->data;
		int cnt = mTextures.size();
		for (int i = 0; i < cnt; ++i)
			_t[i] = mFuncConvertTex(i, pVert[_idx].t0);
		return cnt;
	}
	case RES_ID(0x83, 0x18b):
	{
		LayoutC* pVert = (LayoutC*)mpVerticies->data;
		int cnt = mTextures.size();
		for (int i = 0; i < cnt; ++i)
			_t[i] = mFuncConvertTex(i, pVert[_idx].t0 + 4*i );
		return cnt;
	}
	case RES_ID(0xca, 0x110):
	{
		LayoutD* pVert = (LayoutD*)mpVerticies->data;
		_t[0] = mFuncConvertTex(0, (unsigned char*)pVert[_idx].t0);
		return 1;
	}
	case RES_ID(0x2f, 0x15f):
	{
		LayoutB* pVert = (LayoutB*)mpVerticies->data;
		_t[0] = mFuncConvertTex(0, (unsigned char*)pVert[_idx].t0);
		_t[1] = mFuncConvertTex(1, (unsigned char*)pVert[_idx].t0);
		_t[2] = mFuncConvertTex(2, (unsigned char*)pVert[_idx].t0);
		_t[3] = mFuncConvertTex(3, (unsigned char*)pVert[_idx].t0);
		_t[4] = mFuncConvertTex(4, (unsigned char*)pVert[_idx].t0);
		return 5;
	}
	case RES_ID(0x1d, 0x137):
	{
		LayoutE* pVert = (LayoutE*)mpVerticies->data;
		_t[0] = mFuncConvertTex(0, (unsigned char*)pVert[_idx].t0);
		return 1;
	}
	case RES_ID(0xd2, 0xe5):
	{
		LayoutF* pVert = (LayoutF*)mpVerticies->data;
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
Vector4n jParserD3::GetMatIdx(int _idx)
{
	_warnif(_idx >= mVertexCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	switch (mLayoutFileID)
	{
	case RES_ID(0x59, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pData->data;
		Vector4n ret = Vector4n(pVert[_idx].i[0], pVert[_idx].i[1], pVert[_idx].i[2], pVert[_idx].i[3]);
		return ret;
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	default:
		_warn();
		break;
	}
	return Vector4n(0, 0, 0, 0);
}
Vector4f jParserD3::GetMatWeight(int _idx)
{
	_warnif(_idx >= mVertexCount);

	MyRes_CreateBuffer* pData = mpVerticies;
	switch (mLayoutFileID)
	{
	case RES_ID(0x59, 0x167):
	{
		LayoutA* pVert = (LayoutA*)pData->data;
		Vector4f ret = Vector4f(pVert[_idx].w[0], pVert[_idx].w[1], pVert[_idx].w[2], 0);
		return ret;
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	default:
		_warn();
		break;
	}
	return Vector4f(1, 0, 0, 0);
}