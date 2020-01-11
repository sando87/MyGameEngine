#include "jBitmap.h"

#define _LOBYTE(w)           ((u8)(((u32)(w)) & 0xff))
#define _HIBYTE(w)           ((u8)((((u32)(w)) >> 8) & 0xff))
#define _RGB(r,g,b)          ((u32)(((u8)(r)|((u16)((u8)(g))<<8))|(((u32)(u8)(b))<<16)))
#define _GetRValue(rgb)      (_LOBYTE(rgb))
#define _GetGValue(rgb)      (_LOBYTE(((u16)(rgb)) >> 8))
#define _GetBValue(rgb)      (_LOBYTE((rgb)>>16))

#pragma pack(push, 1)
typedef struct _tagBITMAPFILEHEADER {
	u16    bfType;
	u32   bfSize;
	u16    bfReserved1;
	u16    bfReserved2;
	u32   bfOffBits;
} _BITMAPFILEHEADER;
typedef struct _tagBITMAPINFOHEADER {
	u32      biSize;
	u32       biWidth;
	u32       biHeight;
	u16       biPlanes;
	u16       biBitCount;
	u32      biCompression;
	u32      biSizeImage;
	u32       biXPelsPerMeter;
	u32       biYPelsPerMeter;
	u32      biClrUsed;
	u32      biClrImportant;
} _BITMAPINFOHEADER;
#pragma pack(pop)

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
