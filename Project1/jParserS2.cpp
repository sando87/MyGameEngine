#include "jParserS2.h"

jParserS2::jParserS2()
{
}


jParserS2::~jParserS2()
{
}

bool jParserS2::Init(int _fileIdx)
{
	LoadResources(1);

	mFileIndex = _fileIdx;
	string name = PATH_RESOURCE + to_string(_fileIdx) + "_D3D9RenderContext.bin";
	int size = 0;
	char* pBuf = nullptr;
	jUtils::LoadFile(name, &size, (char**)&pBuf);
	memcpy(&mContextD3D9, pBuf, sizeof(mContextD3D9));

	//if (mContextD3D9.vb_OffsetBytes > 0)
	//{
	//	printf("[%d] offset skip\n", mFileIndex);
	//	return false;
	//}

	if (mContextD3D9.vb_stride == 0)
	{
		printf("[%d] stride zero skip\n", mFileIndex);
		return false;
	}

	for (int i = 0; i < 32; ++i)
	{
		void* pAddr = mContextD3D9.tex_addr[i];
		if (mMapRes.find(pAddr) != mMapRes.end())
		{
			MyResBase* pDataTex = mMapRes[pAddr].first;
			((MyRes_CreateTexture*)pDataTex)->SetSubResMem();
		}
	}

	mTextures.push_back(4); // or 5

	if (!InitGeoInfo())
		return false;

	return true;
}

ID3D11Buffer * jParserS2::GetResIndexBuffer()
{
	return (ID3D11Buffer*)CreateD3DRescource(mContextD3D9.ib_addr);
}

ID3D11ShaderResourceView * jParserS2::GetResShaderResourceView(int _idx)
{
	if (mTextures.size() == 0)
		return nullptr;

	int idx = mTextures[_idx];
	return (ID3D11ShaderResourceView*)CreateD3DRescource(mContextD3D9.tex_addr[idx]);
}

bool jParserS2::InitGeoInfo()
{
	if (mContextD3D9.vb_stride == 0)
	{
		printf("[%d] stride zero\n", mFileIndex);
		return false;
	}

	mGeoInfo.vertexStride = mContextD3D9.vb_stride;
	mGeoInfo.vertexVertexByteOffset = mContextD3D9.vb_OffsetBytes;
	mGeoInfo.vertexTotalCount = mContextD3D9.vb_desc.Size / mGeoInfo.vertexStride;

	mGeoInfo.indiciesIndexUnit = 2;
	mGeoInfo.indiciesCntPerPoly = 3;
	mGeoInfo.indiciesTotalSize = mContextD3D9.ib_desc.Size;
	mGeoInfo.primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; //mContext.draw_primitiveType

	mGeoInfo.drawIndexCount = mContextD3D9.draw_primCount *mGeoInfo.indiciesCntPerPoly;
	mGeoInfo.drawVertCount = mContextD3D9.draw_NumVertices;
	mGeoInfo.drawIndexOffset = mContextD3D9.draw_startIndex;
	mGeoInfo.drawVertOffset = mContextD3D9.draw_BaseVertexIndex;
	return true;
}

Vector3f jParserS2::GetPos(int _idx, int byteOffset)
{
	MyD3DDECLTYPE type;
	int posOff = mContextD3D9.GetPositionOffset(type);
	if (posOff < 0)
		return Vector3f(0, 0, 0);

	MyRes_D3D9_VB* pData = (MyRes_D3D9_VB*)mMapRes[mContextD3D9.vb_addr].first;
	int off = mContextD3D9.vb_stride * _idx + byteOffset + posOff;
	if (type == MyD3DDECLTYPE::D3DDECLTYPE_FLOAT3)
	{
		Vector3f pos;
		memcpy(&pos, &pData->data[off], sizeof(pos));
		return pos;
	}
	else if (type == MyD3DDECLTYPE::D3DDECLTYPE_FLOAT4)
	{
		Vector4f tmp;
		memcpy(&tmp, &pData->data[off], sizeof(tmp));
		return Vector3f(tmp.x, tmp.y, tmp.z);
	}
	else if (type == MyD3DDECLTYPE::D3DDECLTYPE_SHORT2N)
	{
		Vector3f pos;
		unsigned short us[2] = { 0, };
		memcpy(us, &pData->data[off], sizeof(unsigned short) * 2);
		pos.x = (us[0] / 32767.0f) * 16.0f;
		pos.y = (us[1] / 32767.0f) * 16.0f;
		pos.z = 0;
		return pos;
	}
	else
	{
		printf("%d_undefined position\n", mFileIndex);
	}
	return Vector3f(0, 0, 0);
}

Vector3f jParserS2::GetNor(int _idx, int byteOffset)
{
	MyD3DDECLTYPE type;
	int norOff = mContextD3D9.GetNormalOffset(type);
	if (norOff < 0)
		return Vector3f(0, 0, 1);

	int dataOffset = mContextD3D9.vb_stride * _idx + byteOffset + norOff;
	MyRes_D3D9_VB* pData = (MyRes_D3D9_VB*)mMapRes[mContextD3D9.vb_addr].first;
	if (type == MyD3DDECLTYPE::D3DDECLTYPE_UBYTE4N)
	{
		unsigned char nor[4] = { 0, };
		memcpy(nor, &pData->data[dataOffset], 4);
		Vector3f normal;
		normal.x = ((float)nor[0] / 128.0f) - 1.0f;
		normal.y = ((float)nor[1] / 128.0f) - 1.0f;
		normal.z = ((float)nor[2] / 128.0f) - 1.0f;
		return normal;
	}
	else
	{
		printf("%d_undefined normal\n", mFileIndex);
	}

	return Vector3f(0, 0, 1);
}

int jParserS2::GetTex(int _idx, Vector2f * _t, int byteOffset)
{
	MyD3DDECLTYPE type;
	int texelOffset = mContextD3D9.GetTexelOffset(0, type);
	if (texelOffset < 0)
		return 1;

	int dataOffset = mContextD3D9.vb_stride * _idx + byteOffset + texelOffset;
	MyRes_D3D9_VB* pData = (MyRes_D3D9_VB*)mMapRes[mContextD3D9.vb_addr].first;
	if (type == MyD3DDECLTYPE::D3DDECLTYPE_SHORT2N)
	{
		unsigned short utexel[2] = { 0, };
		memcpy(utexel, &pData->data[dataOffset], sizeof(unsigned short) * 2);
		Vector2f texel;
		texel.x = (utexel[0] / 32767.0f) * 16.0f;
		texel.y = (utexel[1] / 32767.0f) * 16.0f;
		_t[0] = texel;
	}
	else if (type == MyD3DDECLTYPE::D3DDECLTYPE_FLOAT2)
	{
		Vector2f texel;
		memcpy(&texel, &pData->data[dataOffset], sizeof(float) * 2);
		_t[0] = texel;
	}
	else if (type == MyD3DDECLTYPE::D3DDECLTYPE_SHORT4)
	{
		short utexel[4] = { 0, };
		memcpy(utexel, &pData->data[dataOffset], sizeof(short) * 4);
		Vector2f texel;
		texel.x = (utexel[0] / 32767.0f) * 16.0f;
		texel.y = (utexel[1] / 32767.0f) * 16.0f;
		_t[0] = texel;
	}
	else
	{
		printf("%d_undefined texel\n", mFileIndex);
	}
	return 1;
}

Vector4n jParserS2::GetMatIdx(int _idx)
{
	return Vector4n();
}

Vector4f jParserS2::GetMatWeight(int _idx)
{
	return Vector4f();
}

char * jParserS2::GetIndiciesData()
{
	void* ibAddr = mContextD3D9.ib_addr;
	MyRes_D3D9_IB* pDataIB = (MyRes_D3D9_IB*)mMapRes[ibAddr].first;
	return pDataIB->data;
}

void * jParserS2::GetTexResAddr(int _idx)
{
	if (_idx >= mTextures.size())
		return nullptr;

	return mContextD3D9.tex_addr[mTextures[_idx]];
}

