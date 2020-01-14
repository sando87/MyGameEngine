#pragma once

#include "junks.h"
#include "jRect3D.h"

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

class jBitmap
{
	u32*    Colors;
	u32    Width;
	u32    Height;

public:
	u32 GetWidth() { return Width; };
	u32 GetHeight() { return Height; };

	jBitmap();
	~jBitmap();
	u32& Access(int x, int y);
	bool Load(const char* FileName);
	bool Save(const char* FileName);
	void RedScale();
	static bool Save(int width, int height, int rowpitch, int byteperpixel, char* data, const char* outfilename);
	static bool SaveAlpha(jRect3D rt, int step, int srcWidth, char* srcData, string fullname);
};
