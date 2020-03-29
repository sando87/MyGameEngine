#include "jZMapLoader.h"
#include "jBitmap.h"

#define GRID_SIZE 240

bool jZMapLoader::LoadHeights(string name)
{
	string filter = name + "_*.heights";
	string fullname = jUtils::FindFile(filter);
	chars file = jUtils::LoadFile2(fullname);
	_warnif(!file);
	if (!file)
	{
		Heights.resize(GRID_SIZE * GRID_SIZE);
		return false;
	}

	string filename = jUtils::GetFilename(fullname);
	strings pieces = jUtils::Split2(filename, "_");
	_warnif(pieces->size() != 5);
	int posX = jUtils::ToDouble(pieces[0]);
	int posY = jUtils::ToDouble(pieces[1]);
	int step = jUtils::ToDouble(pieces[2]);
	int minZ = jUtils::ToDouble(pieces[3]);
	int maxZ = jUtils::ToDouble(pieces[4]);
	Header.step = step;
	Header.minZ = minZ;
	Header.maxZ = maxZ;

	int dataOff = sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 1024;
	int dataSize = file->size() - dataOff;
	u8* pData = (u8*)&file[0] + dataOff;
	Heights.resize(dataSize);
	float sizeZ = Header.maxZ - Header.minZ;
	for (int i = 0; i < dataSize; ++i)
	{
		float rate = pData[i] / 255.0f;
		Heights[i] = Header.minZ + (rate * sizeZ);
	}

	Vector3 min(posX, posY, Header.minZ);
	Vector3 max(posX + GRID_SIZE, posY + GRID_SIZE, Header.maxZ);
	Rect3D.SetRect(min, max - min);
	Filename = name;
	return true;
}

bool jZMapLoader::LoadAccessables(string name)
{
	string filter = name + "_*.aces";
	string fullname = jUtils::FindFile(filter);
	chars file = jUtils::LoadFile2(fullname);
	_warnif(!file);
	if (!file)
	{
		Accessable.resize(GRID_SIZE * GRID_SIZE);
		return false;
	}

	int dataOff = sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 1024;
	int dataSize = file->size() - dataOff;
	u8* pData = (u8*)&file[0] + dataOff;
	Accessable.resize(dataSize);
	for (int i = 0; i < dataSize; ++i)
		Accessable[i] = pData[i] == 0 ? false : true;

	Filename = name;
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
	if (idx < 0 || idx >= Heights.size())
		return false;

	outHeight = Heights[idx];
	return Accessable[idx];
}

bool jZMapLoader::Save(string fullname, jRect3D rt, int step, u32 * pixels, int imgWidth)
{
	int cnt = (rt.Max().x - rt.Min().x) / step;
	int size = sizeof(ZMapHeader) + (sizeof(float) * cnt * cnt);
	ZMapHeader *outbuf = (ZMapHeader *)malloc(size);
	memset(outbuf, 0x00, size);
	outbuf->step = step;
	outbuf->minZ = rt.Min().z;
	outbuf->maxZ = rt.Max().z;

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
