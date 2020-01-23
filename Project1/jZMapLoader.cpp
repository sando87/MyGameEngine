#include "jZMapLoader.h"
#include "jBitmap.h"

bool jZMapLoader::Load(string fullname)
{
	if (jUtils::GetFileExtension(fullname) != "zmap")
		return false;

	chars file = jUtils::LoadFile2(fullname);
	if (!file)
		return false;

	int zmapHeadOff = sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 1024;
	int dataOff = sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 1024 + sizeof(ZMapHeader);

	memcpy(&Header, &file[0] + zmapHeadOff, sizeof(Header));
	int dataSize = file->size() - dataOff;
	u8* pData = (u8*)&file[0] + dataOff;
	Map.resize(dataSize);
	float sizeZ = Header.maxZ - Header.minZ;
	for (int i = 0; i < dataSize; ++i)
	{
		float rate = pData[i] / 255.0f;
		Map[i] = pData[i] == 0 ? -1000.0f : Header.minZ + (rate * sizeZ);
	}

	Vector3 min(Header.minX, Header.minY, Header.minZ);
	Vector3 max(Header.maxX, Header.maxY, Header.maxZ);
	Rect3D.SetRect(min, max - min);
	Filename = jUtils::GetFilenameOnly(fullname);
	return true;
}

bool jZMapLoader::GetHeight(float worldX, float worldY, float& outHeight)
{
	if (!Rect3D.TopBottom().Contains(Vector2(worldX, worldY)))
		return false;

	float localX = worldX - Rect3D.Min().x;
	float localY = worldY - Rect3D.Min().y;
	int idxX = localX / Header.step;
	int idxY = localY / Header.step;
	int pitchCount = Rect3D.Size().x / Header.step;
	int idx = idxY * pitchCount + idxX;
	if (idx < 0 || idx >= Map.size() || Map[idx] == -1000.0f)
		return false;

	outHeight = Map[idx];
	return true;
}

bool jZMapLoader::Save(string fullname, jRect3D rt, int step, u32 * pixels, int imgWidth)
{
	int cnt = (rt.Max().x - rt.Min().x) / step;
	int size = sizeof(ZMapHeader) + (sizeof(float) * cnt * cnt);
	ZMapHeader *outbuf = (ZMapHeader *)malloc(size);
	memset(outbuf, 0x00, size);
	outbuf->ext[0] = 'z'; outbuf->ext[1] = 'm'; outbuf->ext[2] = 'a'; outbuf->ext[3] = 'p';
	outbuf->size = size;
	outbuf->step = step;
	outbuf->minX = rt.Min().x; outbuf->minY = rt.Min().y; outbuf->minZ = rt.Min().z;
	outbuf->maxX = rt.Max().x; outbuf->maxY = rt.Max().y; outbuf->maxZ = rt.Max().z;

	struct tmpXYZW { u8 x;	u8 y;	u8 z;	u8 w;	};
	float minZ = rt.Min().z;
	float maxZ = rt.Max().z;
	float sizeZ = maxZ - minZ;
	tmpXYZW * curPixels = (tmpXYZW *)pixels;
	float *curHeight = (float *)((char *)outbuf + sizeof(ZMapHeader));
	for (int y = cnt - 1; y >= 0; --y)  //texture y axis up-side-down compared to world y axis
	{
		int off = imgWidth * y;
		for (int x = 0; x < cnt; ++x)
		{
			float rate = (255 - curPixels[off + x].w) / 255.0f;
			float worldHeight = minZ + (rate * sizeZ);
			*curHeight = worldHeight;
			curHeight++;
		}
	}

	jUtils::SaveToFile(fullname, (char *)outbuf, size);
	free(outbuf);
	return true;
}
