#include "jImage.h"



jImage::jImage(string _name)
{
	mWidth = 0;
	mHeight = 0;
	mFileName = "";
	if (_name.length() > 0)
		Load(_name);
}

jImage::~jImage()
{
}

bool jImage::Load(string name)
{
	mBuffer = jUtils::LoadTarga(name, mHeight, mWidth);
	if (!mBuffer)
		return false;

	mFileName = name;
	return true;
}
