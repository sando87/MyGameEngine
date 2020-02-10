#include "jBitmap.h"
#include "jZMapLoader.h"


jBitmap::jBitmap()
{
	Colors = nullptr;
	Width = Height = 0;
}
jBitmap::~jBitmap()
{
	if (Colors)
		delete[] Colors;
}
u32& jBitmap::Access(int x, int y)
{
	return Colors[x*Height + y];
}
bool jBitmap::Load(const char* FileName)
{
	_BITMAPFILEHEADER    BitmapFileHeader;
	_BITMAPINFOHEADER    BitmapInfoHeader;

	ifstream infile(FileName, ios::binary);
	infile.read((char*)&BitmapFileHeader, sizeof(BitmapFileHeader));
	infile.read((char*)&BitmapInfoHeader, sizeof(BitmapInfoHeader));

	if (BitmapFileHeader.bfType != 0x4d42
		|| BitmapInfoHeader.biClrUsed != 0
		|| BitmapInfoHeader.biBitCount != 24
		|| BitmapInfoHeader.biCompression != 0
		|| BitmapInfoHeader.biPlanes != 1)return false;

	Width = BitmapInfoHeader.biWidth;
	Height = BitmapInfoHeader.biHeight;

	u8* ColorsData = new u8[3 * Width*Height];
	Colors = new u32[Width*Height];

	infile.read((char*)ColorsData, 3 * Width*Height);

	u32    Offset = 0;

	for (int y = Height - 1;y >= 0;y--)
	{
		for (int x = 0;x<(int)Width;x++, Offset += 3)
		{
			u8 B = ColorsData[Offset + 0];
			u8 G = ColorsData[Offset + 1];
			u8 R = ColorsData[Offset + 2];
			Access(x, y) = _RGB(R, G, B);
		}
	}

	delete[] ColorsData;

	infile.close();
	return true;
}
bool jBitmap::Save(const char* FileName)
{
	if (Colors == nullptr)
		return false;

	_BITMAPFILEHEADER    BitmapFileHeader = {
		0x4d42, //Bmp Mark
		sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 3 * Width * Height, //File Size
		0,    //Not Used
		0,    //Not Used
		sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER)    //Colors Offset in File
	};

	_BITMAPINFOHEADER    BitmapInfoHeader = {
		sizeof(_BITMAPINFOHEADER),    //Structure Size
		Width,
		Height,
		1,        //Number of Plans
		24,        //Bits Per colors 
		0,        //Compression Ration
		0,        //Original Image Size (When Compressed)
		2835,    //Number of Pixel Per Meter (Vertical)
		2835,    //Number of Pixel Per Meter (Horizontal)
		0,        //Number of Used Colors (Indexed Mode)
		0,        //Number of Important Colors (Indexed Mode)
	};

	u8* ColorsData = new u8[3 * Width*Height];

	u32    Offset = 0;

	for (int y = Height - 1;y >= 0;y--)
	{
		for (int x = 0;x<(int)Width;x++, Offset += 3)
		{
			u32    Color = Access(x, y);
			ColorsData[Offset + 0] = _GetBValue(Color);
			ColorsData[Offset + 1] = _GetGValue(Color);
			ColorsData[Offset + 2] = _GetRValue(Color);
		}
	}


	ofstream outfile(FileName, ios::binary);
	outfile.write((char*)&BitmapFileHeader, sizeof(BitmapFileHeader));
	outfile.write((char*)&BitmapInfoHeader, sizeof(BitmapInfoHeader));
	outfile.write((char*)ColorsData, 3 * Width*Height);
	outfile.close();

	delete[] ColorsData;
	return false;
}
bool jBitmap::Save(int width, int height, int rowpitch, int byteperpixel, char* data, const char* outfilename)
{
	_BITMAPFILEHEADER    BitmapFileHeader = {
		0x4d42, //Bmp Mark
		sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + byteperpixel * width * height, //File Size
		0,    //Not Used
		0,    //Not Used
		sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER)    //Colors Offset in File
	};

	_BITMAPINFOHEADER    BitmapInfoHeader = {
		sizeof(_BITMAPINFOHEADER),    //Structure Size
		(u32)width,
		(u32)height,
		1,        //Number of Plans
		(u16)(byteperpixel * 8),        //Bits Per colors 
		0,        //Compression Ration
		0,        //Original Image Size (When Compressed)
		2835,    //Number of Pixel Per Meter (Vertical)
		2835,    //Number of Pixel Per Meter (Horizontal)
		0,        //Number of Used Colors (Indexed Mode)
		0,        //Number of Important Colors (Indexed Mode)
	};

	int size = byteperpixel * width * height;
	char* ColorsData = new char[size];
	memset(ColorsData, 0x00, size);

	int rowSize = width * byteperpixel;
	for (int y = 0; y< height; ++y)
	{
		char* offSrc = data + (rowpitch * y);
		char* offDesc = ColorsData + (rowSize * (height - 1 - y));
		memcpy(offDesc, offSrc, rowSize);
	}

	ofstream outfile(outfilename, ios::binary);
	outfile.write((char*)&BitmapFileHeader, sizeof(BitmapFileHeader));
	outfile.write((char*)&BitmapInfoHeader, sizeof(BitmapInfoHeader));
	outfile.write((char*)ColorsData, byteperpixel * width * height);
	outfile.close();

	delete[] ColorsData;
	return false;
}
bool jBitmap::SaveAlpha(jRect3D rt, int step, int srcWidth, char* srcData, string fullname)
{
	int cnt = (rt.Max().x - rt.Min().x) / step;
	int byteperpixel = 1;
	int dataSize = cnt * cnt * byteperpixel;
	//int totalSize = sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 1024 + sizeof(ZMapHeader) + dataSize;
	int totalSize = sizeof(_BITMAPFILEHEADER) + sizeof(_BITMAPINFOHEADER) + 1024 + dataSize;

	_BITMAPFILEHEADER    BitmapFileHeader = {
		0x4d42, //Bmp Mark
		(u32)totalSize, //File Size
		0,    //Not Used
		0,    //Not Used
		(u32)(totalSize - dataSize)     //Colors Offset in File
	};

	_BITMAPINFOHEADER    BitmapInfoHeader = {
		sizeof(_BITMAPINFOHEADER),    //Structure Size
		(u32)cnt,
		(u32)cnt,
		1,        //Number of Plans
		(u16)(byteperpixel * 8),        //Bits Per colors 
		0,        //Compression Ration
		0,        //Original Image Size (When Compressed)
		2835,    //Number of Pixel Per Meter (Vertical)
		2835,    //Number of Pixel Per Meter (Horizontal)
		0,        //Number of Used Colors (Indexed Mode)
		0,        //Number of Important Colors (Indexed Mode)
	};

	//ZMapHeader zmapHead;
	//memset(&zmapHead, 0x00, sizeof(zmapHead));
	//zmapHead.ext[0] = 'z'; zmapHead.ext[1] = 'm'; zmapHead.ext[2] = 'a'; zmapHead.ext[3] = 'p';
	//zmapHead.size = sizeof(ZMapHeader);
	//zmapHead.step = step;
	//zmapHead.minX = rt.Min().x; zmapHead.minY = rt.Min().y; zmapHead.minZ = rt.Min().z;
	//zmapHead.maxX = rt.Max().x; zmapHead.maxY = rt.Max().y; zmapHead.maxZ = rt.Max().z;

	struct tmpXYZW { u8 x;	u8 y;	u8 z;	u8 w; };
	tmpXYZW colorTable[256] = { 0, };
	for (int i = 0; i < 256; ++i)
	{
		colorTable[i].x = i;
		colorTable[i].y = i;
		colorTable[i].z = i;
		colorTable[i].w = 0;
	}

	chars outData;
	outData->resize(dataSize);
	float minZ = rt.Min().z;
	float maxZ = rt.Max().z;
	float sizeZ = maxZ - minZ;
	tmpXYZW * curSrc = (tmpXYZW *)srcData;
	char *curDest = &outData[0];
	for (int y = cnt - 1; y >= 0; --y)  //texture y axis up-side-down compared to world y axis
	{
		int off = srcWidth * y;
		for (int x = 0; x < cnt; ++x)
		{
			curDest[0] = 255 - curSrc[off + x].w;
			//curDest[1] = 255 - curSrc[off + x].w;
			curDest += 1;
		}
	}

	chars outBuf;
	outBuf->resize(totalSize);
	char* curPos = &outBuf[0];
	memcpy(curPos, (char *)&BitmapFileHeader, sizeof(BitmapFileHeader));
	curPos += sizeof(BitmapFileHeader);
	memcpy(curPos, (char *)&BitmapInfoHeader, sizeof(BitmapInfoHeader));
	curPos += sizeof(BitmapInfoHeader);
	memcpy(curPos, (char *)&colorTable, 1024);
	curPos += 1024;
	//memcpy(curPos, (char *)&zmapHead, sizeof(zmapHead));
	//curPos += sizeof(zmapHead);
	memcpy(curPos, (char *)&outData[0], dataSize);

	jUtils::SaveToFile(fullname, (char *)&outBuf[0], totalSize);
	return true;
}
void jBitmap::RedScale()
{
	u32 R;
	for (int y = 0;y<(int)Height;y++)
	{
		for (int x = 0;x<(int)Width;x++)
		{
			u32    Color = Access(x, y);
			R = _GetRValue(Color);
			Access(x, y) = _RGB(R, 0, 0);
		}
	}

}
