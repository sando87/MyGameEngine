#include "jObjectMeta.h"



jObjectMeta::jObjectMeta()
{
}


jObjectMeta::~jObjectMeta()
{
}

bool jObjectMeta::Load(string name)
{
	string metaPath = PATH_RESOURCES + string("meta/");
	strings lines = jUtils::LoadLines(metaPath + name);
	if (!lines)
		return false;

	filename = name;
	objname = lines[0];
	type_shader = lines[1];
	alpha = lines[2] == "TRUE" ? true : false;
	depth = lines[3] == "TRUE" ? true : false;

	strings pieces = jUtils::Split2(lines[4], " ");
	Vector3 position;
	position.x = jUtils::ToDouble(pieces[0]);
	position.y = jUtils::ToDouble(pieces[1]);
	position.z = jUtils::ToDouble(pieces[2]);
	worldPos = position;

	animName = lines[5];

	for (int i = 6; i < lines->size(); ++i)
	{
		strings columns = jUtils::Split2(lines[i], " ");
		if (columns->size() >= 3)
		{
			Vector4f pt;
			pt.x = jUtils::ToDouble(columns[0]);
			pt.y = jUtils::ToDouble(columns[1]);
			pt.z = jUtils::ToDouble(columns[2]);
			pt.w = 0;
			texels.push_back(pt);
		}
		else
		{
			imgFileNames.push_back(columns[0]);
		}
	}

	return true;
}

bool jZMapLoader::Load(string fullname)
{
	if (jUtils::GetFileExtension(fullname) != "zmap")
		return false;

	chars file = jUtils::LoadFile2(fullname);
	if (!file)
		return false;

	memcpy(&Header, &file[0], sizeof(Header));
	int dataSize = file->size() - sizeof(Header);
	char* pData = (char *)&file[0] + sizeof(Header);
	Map.resize(dataSize / sizeof(float));
	memcpy(&Map[0], pData, dataSize);

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
	if (idx < 0 || idx >= Map.size())
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
