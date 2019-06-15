#include "jParserD3.h"
#include "jUtils.h"
#include "jLog.h"
#include <sstream>

#define RES_ID(crc, size) (((size)<<8) | (crc))

map<void*, pair<MyResBase*, void*>> jParserD3::mMapRes;

struct Layout_59_167
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
struct Layout_A_10E //same as 5D_15F
{
	float p[3];
	unsigned char n[4];
	unsigned char c0[4];
	unsigned char c1[4];
	unsigned char t0[4];
	unsigned char t1[4];
};

jParserD3::jParserD3()
{
	mTexCoordIndex = 0;
	mTextureIndex = 0;
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
		jUtils::LoadFile(path + _str, &filesize, (char**)&pBuf);
		
		mMapRes[id] = make_pair(pBuf, nullptr);
		return true;
	});
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


void jParserD3::Init(int _fileIdx)
{
	LoadResources();

	mFileIndex = _fileIdx;
	string name = PATH_RESOURCE + to_string(_fileIdx) + "_RenderingContext.bin";
	int size = 0;
	char* pBuf = nullptr;
	jUtils::LoadFile(name, &size, (char**)&pBuf);
	memcpy(&mContext, pBuf, sizeof(mContext));

	void * addr = mContext.layout_addr;
	MyRes_CreateLayout* pData = (MyRes_CreateLayout*)mMapRes[addr].first;
	pData->SetNameOffset();
	//D3D11_INPUT_ELEMENT_DESC* pLayout = (D3D11_INPUT_ELEMENT_DESC*)pData->data;
	//int num = pData->head.reserve1;
	//for (int i = 0; i < num; ++i) {}


	if (mContext.vb[mVertBufIdx].numBuf != 1)
		_warn();

	int vertexBufSize = mMapRes[mContext.vb[mVertBufIdx].addr].first->totalSize - sizeof(MyRes_CreateBuffer);
	mVertexStride = mContext.vb[mVertBufIdx].strides[0];
	mVertexOffset = mContext.vb[mVertBufIdx].offset[0];
	mVertexCount = vertexBufSize / mVertexStride;

	SetTexCoordIndex();
	SetTextureIndex();
}
void jParserD3::CreateD3DRescource(void* addr)
{
	if (mMapRes.find(addr) == mMapRes.end())
		return;

	MyResBase* pData = mMapRes[addr].first;
	void* pIF = mMapRes[addr].second;
	if (pIF != nullptr)
		return;

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
}
CBMain jParserD3::GetCBMain()
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
	return ret;
}

ID3D11Buffer* jParserD3::GetResIndexBuffer()
{
	CreateD3DRescource(mContext.ib_addr);
	return (ID3D11Buffer*)mMapRes[mContext.ib_addr].second;
}
ID3D11ShaderResourceView* jParserD3::GetResShaderResourceView()
{
	CreateD3DRescource(mContext.tex[mTextureIndex].addr);
	return (ID3D11ShaderResourceView*)mMapRes[mContext.tex[mTextureIndex].addr].second;
}
void jParserD3::SetTexCoordIndex()
{
	MyRes_CreateShader* pData = (MyRes_CreateShader*)mMapRes[mContext.vs_addr].first;
	int resID = RES_ID(pData->head.crc, pData->head.totalSize);
	switch (resID)
	{
	case RES_ID(0xCB, 0x11BC):
	case RES_ID(0x53, 0x1318):
		mTexCoordIndex = 0;
		break;
	case RES_ID(0x84, 0x1A30):
		mTexCoordIndex = 1;
		break;
	default:
		_warn();
		break;
	}
}
void jParserD3::SetTextureIndex()
{
	MyRes_CreateShader* pData = (MyRes_CreateShader*)mMapRes[mContext.ps_addr].first;
	int resID = RES_ID(pData->head.crc, pData->head.totalSize);
	switch (resID)
	{
	case RES_ID(0xF2, 0x1044):
		mTextureIndex = 0;
		break;
	case RES_ID(0x1C, 0x2B70):
		mTextureIndex = 1;
		break;
	default:
		_warn();
		break;
	}
}
Vector3f jParserD3::GetPos(int _idx)
{
	_warnif(_idx >= mVertexCount);

	void * addr = mContext.vb[mVertBufIdx].addr;
	MyRes_CreateBuffer* pData = (MyRes_CreateBuffer*)mMapRes[addr].first;
	MyRes_CreateLayout* pDataLayout = (MyRes_CreateLayout*)mMapRes[mContext.layout_addr].first;
	int resID = RES_ID(pDataLayout->head.crc, pDataLayout->head.totalSize);
	switch (resID)
	{
	case RES_ID(0x59, 0x167):
	{
		Layout_59_167* pVert = (Layout_59_167*)pData->data;
		return Vector3f(pVert[_idx].p[0], pVert[_idx].p[1], pVert[_idx].p[2]);
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	{
		Layout_A_10E* pVert = (Layout_A_10E*)pData->data;
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

	void * addr = mContext.vb[mVertBufIdx].addr;
	MyRes_CreateBuffer* pData = (MyRes_CreateBuffer*)mMapRes[addr].first;
	MyRes_CreateLayout* pDataLayout = (MyRes_CreateLayout*)mMapRes[mContext.layout_addr].first;
	int resID = RES_ID(pDataLayout->head.crc, pDataLayout->head.totalSize);
	switch (resID)
	{
	case RES_ID(0x59, 0x167):
	{
		Layout_59_167* pVert = (Layout_59_167*)pData->data;
		Vector3f ret;
		ret.x = ((float)pVert[_idx].n[0] / 128.0f) - 1.0f;
		ret.y = ((float)pVert[_idx].n[1] / 128.0f) - 1.0f;
		ret.z = ((float)pVert[_idx].n[2] / 128.0f) - 1.0f;
		return ret;
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	{
		Layout_A_10E* pVert = (Layout_A_10E*)pData->data;
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
Vector2f jParserD3::ConvertTex(unsigned char *_p)
{
	Matrix4 matTex = GetCBMain().matTex[mTexCoordIndex];
	Vector4f tmp1;
	tmp1.x = _p[1];
	tmp1.y = _p[0];
	tmp1.z = _p[3];
	tmp1.w = _p[2];

	Vector4f tmp2;
	tmp2.x = tmp1.y * 0.003906f + tmp1.x;
	tmp2.y = tmp1.w * 0.003906f + tmp1.z;
	tmp2.x = tmp2.x * 0.5f - 64.0f;
	tmp2.y = tmp2.y * 0.5f - 64.0f;
	tmp2.z = 1.0f;

	Vector4f tmp3;
	tmp3.x = tmp2.x * matTex[0] + tmp2.y * matTex[1] + tmp2.z * matTex[3];
	tmp3.y = tmp2.x * matTex[4] + tmp2.y * matTex[5] + tmp2.z * matTex[7];
	//tmp3.z = tmp2.x * matTex[8] + tmp2.y * matTex[9] + tmp2.z * matTex[11];
	//tmp3.w = tmp2.x * matTex[12] + tmp2.y * matTex[13] + tmp2.z * matTex[15];

	return Vector2f(tmp3.x, tmp3.y);
}
Vector2f jParserD3::GetTex(int _idx)
{
	_warnif(_idx >= mVertexCount);

	void * addr = mContext.vb[mVertBufIdx].addr;
	MyRes_CreateBuffer* pData = (MyRes_CreateBuffer*)mMapRes[addr].first;
	MyRes_CreateLayout* pDataLayout = (MyRes_CreateLayout*)mMapRes[mContext.layout_addr].first;
	int resID = RES_ID(pDataLayout->head.crc, pDataLayout->head.totalSize);
	switch (resID)
	{
	case RES_ID(0x59, 0x167):
	{
		Layout_59_167* pVert = (Layout_59_167*)pData->data;
		Vector2f ret = ConvertTex(pVert[_idx].t0);
		return ret;
	}
	case RES_ID(0x5D, 0x15F):
	case RES_ID(0xA, 0x10E):
	{
		Layout_A_10E* pVert = (Layout_A_10E*)pData->data;
		Vector2f ret = ConvertTex(pVert[_idx].t0);
		return ret;
	}
	default:
		_warn();
		break;
	}
	return Vector2f(0, 0);
}
Vector4n jParserD3::GetMatIdx(int _idx)
{
	_warnif(_idx >= mVertexCount);

	void * addr = mContext.vb[mVertBufIdx].addr;
	MyRes_CreateBuffer* pData = (MyRes_CreateBuffer*)mMapRes[addr].first;
	MyRes_CreateLayout* pDataLayout = (MyRes_CreateLayout*)mMapRes[mContext.layout_addr].first;
	int resID = RES_ID(pDataLayout->head.crc, pDataLayout->head.totalSize);
	switch (resID)
	{
	case RES_ID(0x59, 0x167):
	{
		Layout_59_167* pVert = (Layout_59_167*)pData->data;
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

	void * addr = mContext.vb[mVertBufIdx].addr;
	MyRes_CreateBuffer* pData = (MyRes_CreateBuffer*)mMapRes[addr].first;
	MyRes_CreateLayout* pDataLayout = (MyRes_CreateLayout*)mMapRes[mContext.layout_addr].first;
	int resID = RES_ID(pDataLayout->head.crc, pDataLayout->head.totalSize);
	switch (resID)
	{
	case RES_ID(0x59, 0x167):
	{
		Layout_59_167* pVert = (Layout_59_167*)pData->data;
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