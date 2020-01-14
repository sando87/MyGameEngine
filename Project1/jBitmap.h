#pragma once

#include "junks.h"
#include "jRect3D.h"

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
