#include "jImage.h"
#include "jCaches.h"
#include "jParserSub.h"
#include "jPng.h"
#include "jBitmap.h"

jImage::jImage(string fullname)
{
	SetFullname(fullname);
}

jImage::~jImage()
{
}

void jImage::OnLoad()
{
	string fullname = GetFullname();
	
	jImage* ret = (jImage*)jCaches::CacheClass(fullname, [](string _fullname) {
		jImage* img = new jImage(_fullname);
		img->LoadImgFile();
		return img;
	});
	
	
	if (ret != nullptr)
	{
		mBuffer = ret->mBuffer;
		mWidth = ret->mWidth;
		mHeight = ret->mHeight;
	}
}

bool jImage::LoadImgFile()
{
	string fullname = GetFullname();
	_exceptif(false == jUtils::ExistFile(fullname), _echoS(fullname); return false;);

	string ext = jUtils::GetFileExtension(fullname);
	if (ext == "dump")
	{
		mBuffer = jUtils::LoadFile2(fullname);
		MyRes_CreateTexture* rawData = (MyRes_CreateTexture*)&mBuffer[0];
		mWidth = rawData->desc.Width;
		mHeight = rawData->desc.Height;
	}
	else if (ext == "tga")
	{
		mBuffer = jUtils::LoadTarga(fullname, mHeight, mWidth);
	}
	else if (ext == "png")
	{
		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, fullname.c_str());
		_errorif(image.empty());

		mBuffer->resize(image.size());
		memcpy(&mBuffer[0], &image[0], image.size());
		mWidth = width;
		mHeight = height;
	}
	else if (ext == "bmp")
	{
		jBitmap bitmap;
		bitmap.Load(fullname.c_str());
		mWidth = bitmap.GetWidth();
		mHeight = bitmap.GetHeight();
		mBuffer->resize(mWidth * mHeight * sizeof(u32));
		memcpy(&mBuffer[0], bitmap.GetBuffer(), mBuffer->size());
	}
	else
	{
		_warn();
		return false;
	}

	return true;
}
